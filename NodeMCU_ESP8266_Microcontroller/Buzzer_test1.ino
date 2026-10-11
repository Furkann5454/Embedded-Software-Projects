#define BUZZER_PIN D5
#define REED_PIN   D1

#define NOTE_C4  262
#define NOTE_D4  294
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_G4  392
#define NOTE_A4  440
#define NOTE_C5  523

int melodi[] = {
  NOTE_G4, NOTE_G4, NOTE_A4, NOTE_G4,
  NOTE_C5, NOTE_A4, NOTE_G4, NOTE_F4,
  NOTE_G4, NOTE_A4, NOTE_C5, NOTE_C5,
  NOTE_A4, NOTE_G4, NOTE_F4, NOTE_G4
};

int sureler[] = {
  200, 200, 300, 300,
  400, 300, 200, 200,
  200, 200, 300, 300,
  400, 300, 200, 500
};

int notaSayisi = sizeof(melodi) / sizeof(melodi[0]);

bool dahaOnceTetiklendi = false;

void melodyCal() {
  for (int i = 0; i < notaSayisi; i++) {
    tone(BUZZER_PIN, melodi[i]);
    delay(sureler[i]);

    noTone(BUZZER_PIN);
    delay(60);
  }

  noTone(BUZZER_PIN);
  digitalWrite(BUZZER_PIN, LOW);
}

void setup() {
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(REED_PIN, INPUT);

  noTone(BUZZER_PIN);
  digitalWrite(BUZZER_PIN, LOW);
}

void loop() {
  int durum = digitalRead(REED_PIN);

  if (durum == LOW && dahaOnceTetiklendi == false) {
    dahaOnceTetiklendi = true;
    melodyCal();
  }

  if (durum == HIGH) {
    dahaOnceTetiklendi = false;
  }

  delay(20);
}