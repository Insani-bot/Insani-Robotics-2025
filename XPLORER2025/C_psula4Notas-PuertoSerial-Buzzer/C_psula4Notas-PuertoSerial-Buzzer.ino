// ======= Librería para notas =======
#define NOTE_C6  1047
#define NOTE_E6  1319
#define NOTE_G6  1568

#define NOTE_A5  880
#define NOTE_E6  1319

#define NOTE_E7  2637

#define NOTE_G4  392
#define NOTE_C5  523

// ======= Pin del buzzer =======
const int buzzer = 11;

void setup() {
  Serial.begin(9600);
  pinMode(buzzer, OUTPUT);

  Serial.println("Listo. Envia A, B, C o D.");
}

// ===== TONOS =====

// A — Tono elegante C6–E6–G6
void tonoA() {
  tone(buzzer, NOTE_C6, 120);
  delay(150);
  tone(buzzer, NOTE_E6, 120);
  delay(150);
  tone(buzzer, NOTE_G6, 120);
  delay(200);
  noTone(buzzer);
}

// B — Ding Ding
void tonoB() {
  tone(buzzer, NOTE_A5, 120);
  delay(150);
  tone(buzzer, NOTE_E6, 150);
  delay(200);
  noTone(buzzer);
}

// C — Micro power-up estilo Mario
void tonoC() {
  tone(buzzer, NOTE_E6, 100);
  delay(120);
  tone(buzzer, NOTE_G6, 100);
  delay(120);
  tone(buzzer, NOTE_E7, 150);
  delay(200);
  noTone(buzzer);
}

// D — OK grave–agudo
void tonoD() {
  tone(buzzer, NOTE_G4, 120);
  delay(150);
  tone(buzzer, NOTE_C5, 180);
  delay(200);
  noTone(buzzer);
}

void loop() {
  if (Serial.available()) {
    char c = Serial.read();

    if (c == 'A') tonoA();
    if (c == 'B') tonoB();
    if (c == 'C') tonoC();
    if (c == 'D') tonoD();
  }
}
