#include <Arduino.h>
#include <DHT.h>

#define DHTPIN D2
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

void setup() {
    Serial.begin(115200);
    dht.begin();

    Serial.println("DHT11 baslatildi!");
}

void loop() {
    float sicaklik = dht.readTemperature();
    float nem = dht.readHumidity();

    if (isnan(sicaklik) || isnan(nem)) {
        Serial.println("DHT11 okuma hatasi!");
    }
    else {
        Serial.print("Sicaklik: ");
        Serial.print(sicaklik);
        Serial.println(" C");

        Serial.print("Nem: %");
        Serial.println(nem);

        Serial.println("----------------");
    }

    delay(2000);
}