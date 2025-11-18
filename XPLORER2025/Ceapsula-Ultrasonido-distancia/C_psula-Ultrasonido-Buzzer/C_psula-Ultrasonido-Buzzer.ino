// Pines ultrasónico
const int TRIG = 13;
const int ECHO = 10;

// Buzzer
const int BUZZER = 11;

long duracion;
int distancia;

void setup() {
  Serial.begin(9600);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(BUZZER, OUTPUT);
}

void loop() {

  // --- MEDIR DISTANCIA ---
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  duracion = pulseIn(ECHO, HIGH);
  distancia = duracion * 0.034 / 2;

  Serial.print("Distancia: ");
  Serial.print(distancia);
  Serial.println(" cm");

  // --- TONOS SEGÚN DISTANCIA ---
  if (distancia >= 30) {
    beep(523, 80);   // C5 Grave
    delay(300);
  }
  else if (distancia >= 20) {
    beep(659, 80);   // E5 Medio
    delay(200);
  }
  else if (distancia >= 10) {
    beep(880, 80);   // A5 Agudo
    delay(120);
  }
  else {
    beep(1047, 50);  // C6 Muy agudo
    delay(60);       // Muy rápido
  }
}

// ---------------------------
// Función beep con tono
// ---------------------------
void beep(int frecuencia, int duracion) {
  tone(BUZZER, frecuencia, duracion);
}
