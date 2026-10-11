#include <Arduino.h>
#include <DHT.h>

#define DHTPIN D2
#define DHTTYPE DHT11
#define BUZZER_PIN D5

#define SICAKLIK_LIMITI 30.0

const unsigned long OTME_SURESI = 3000;
const unsigned long BEKLEME_SURESI = 10000;
const unsigned long SENSOR_ARALIGI = 2000;

DHT dht(DHTPIN, DHTTYPE);

bool alarmAktif = false;
bool buzzerAcik = false;

unsigned long buzzerBaslangic = 0;
unsigned long sensorZamani = 0;

void setup() {
    Serial.begin(115200);
    dht.begin();

    pinMode(BUZZER_PIN, OUTPUT);
    noTone(BUZZER_PIN);
    digitalWrite(BUZZER_PIN, LOW);

    Serial.println("Buzzerli sicaklik sensoru baslatildi!");
}

void loop() {
    unsigned long simdi = millis();

    // Her 2 saniyede bir sicaklik ve nem oku
    if (simdi - sensorZamani >= SENSOR_ARALIGI) {
        sensorZamani = simdi;

        float sicaklik = dht.readTemperature();
        float nem = dht.readHumidity();

        if (isnan(sicaklik) || isnan(nem)) {
            Serial.println("DHT11 okuma hatasi!");
            alarmAktif = false;
        } else {
            Serial.print("Sicaklik: ");
            Serial.print(sicaklik);
            Serial.println(" C");

            Serial.print("Nem: %");
            Serial.println(nem);

            alarmAktif = (sicaklik > SICAKLIK_LIMITI);
        }
    }

    // Sicaklik normalse buzzer kapansin
    if (!alarmAktif) {
        if (buzzerAcik) {
            noTone(BUZZER_PIN);
            buzzerAcik = false;
        }

        buzzerBaslangic = simdi;
        return;
    }

    // Sicaklik yuksekse buzzer dongusu
    unsigned long gecenSure = simdi - buzzerBaslangic;

    if (gecenSure < OTME_SURESI) {
        if (!buzzerAcik) {
            tone(BUZZER_PIN, 2000);
            buzzerAcik = true;
            Serial.println("ALARM: Buzzer 3 saniye otuyor!");
        }
    } else if (gecenSure < OTME_SURESI + BEKLEME_SURESI) {
        if (buzzerAcik) {
            noTone(BUZZER_PIN);
            buzzerAcik = false;
            Serial.println("Buzzer sustu. 10 saniye bekleniyor...");
        }
    } else {
        buzzerBaslangic = simdi;
    }
}