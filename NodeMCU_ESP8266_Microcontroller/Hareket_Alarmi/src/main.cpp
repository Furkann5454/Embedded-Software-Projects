#include <Arduino.h>

#define PIR_PIN D5
#define BUZZER_PIN D6

const unsigned long ALARM_SURESI = 2000;

bool oncekiHareket = LOW;
bool buzzerAcik = false;

unsigned long alarmBaslangici = 0;

void setup() {
    Serial.begin(115200);

    pinMode(PIR_PIN, INPUT);
    pinMode(BUZZER_PIN, OUTPUT);

    noTone(BUZZER_PIN);
    digitalWrite(BUZZER_PIN, LOW);

    Serial.println("Hareket alarm sistemi baslatildi!");
}

void loop() {
    unsigned long simdi = millis();

    bool hareket = digitalRead(PIR_PIN);

    // Yeni hareket algilandiginda
    if (hareket == HIGH && oncekiHareket == LOW) {
        Serial.println("HAREKET ALGILANDI!");

        tone(BUZZER_PIN, 2000);
        buzzerAcik = true;

        alarmBaslangici = simdi;
    }

    // 2 saniye dolunca buzzer susar
    if (buzzerAcik &&
        simdi - alarmBaslangici >= ALARM_SURESI) {

        noTone(BUZZER_PIN);
        buzzerAcik = false;

        Serial.println("Buzzer sustu.");
    }

    // Hareket sinyali sona erdiginde
    if (hareket == LOW && oncekiHareket == HIGH) {
        Serial.println("Hareket sinyali sona erdi.");
    }

    oncekiHareket = hareket;

    delay(50);
}