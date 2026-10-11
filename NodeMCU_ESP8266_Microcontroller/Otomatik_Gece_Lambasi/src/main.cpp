#include <Arduino.h>

#define LDR_PIN A0

void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println("LDR Analog Test Basladi!");
}

void loop() {
    int deger = analogRead(LDR_PIN);

    Serial.print("LDR Analog Deger: ");
    Serial.println(deger);

    delay(500);
}