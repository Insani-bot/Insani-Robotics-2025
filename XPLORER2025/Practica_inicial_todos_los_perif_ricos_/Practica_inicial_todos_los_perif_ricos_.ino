#include <SoftwareSerial.h>
#include <Servo.h>

// === BLUETOOTH ===
SoftwareSerial BT(9, 8); // RX, TX

char cmd = '\0';

// === MOTORES H-BRIDGE ===
const int M1A = 4;
const int M1B = 2;
const int M2A = 12;
const int M2B = 7;

// === RGB LED ===
const int pinR = 6;
const int pinG = 5;
const int pinB = 3;  // !! Aquí solo LED. (Si usas servo en D6, desconecta el LED rojo)

// === ULTRASONIDO ===
const int trigPin = 13;
const int echoPin = 10;

// === SERVO CABEZA ===
Servo cabeza;
const int servoPin = 6;  // Servo en D6 (en el mismo que rojo)

// === BUZZER ===
const int buzzerPin = 11;

// ======================== FUNCIONES =========================

long medirDistancia() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(3);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long dur = pulseIn(echoPin, HIGH, 20000);
  return dur * 0.034 / 2;
}

// === MOTORES ===
void adelante() {
  digitalWrite(M1A, HIGH); digitalWrite(M1B, LOW);
  digitalWrite(M2A, HIGH); digitalWrite(M2B, LOW);
}

void atras() {
  digitalWrite(M1A, LOW); digitalWrite(M1B, HIGH);
  digitalWrite(M2A, LOW); digitalWrite(M2B, HIGH);
}

void izquierda() {
  digitalWrite(M1A, LOW);  digitalWrite(M1B, HIGH);
  digitalWrite(M2A, HIGH); digitalWrite(M2B, LOW);
}

void derecha() {
  digitalWrite(M1A, HIGH); digitalWrite(M1B, LOW);
  digitalWrite(M2A, LOW);  digitalWrite(M2B, HIGH);
}

void stopMotores() {
  digitalWrite(M1A, LOW);
  digitalWrite(M1B, LOW);
  digitalWrite(M2A, LOW);
  digitalWrite(M2B, LOW);
}

// === RGB LED ===
void encenderColor(int r, int g, int b) {
  analogWrite(pinR, r);
  analogWrite(pinG, g);
  analogWrite(pinB, b);
}

// === BUZZER ===
void beep(int freq, int duracion) {
  tone(buzzerPin, freq, duracion);
  delay(duracion);
}

void alarma() {
  tone(buzzerPin, 1500);
}

void apagarBuzzer() {
  noTone(buzzerPin);
}

// ======================== SETUP =========================
void setup() {
  Serial.begin(9600);
  BT.begin(9600);

  // Motores
  pinMode(M1A, OUTPUT);
  pinMode(M1B, OUTPUT);
  pinMode(M2A, OUTPUT);
  pinMode(M2B, OUTPUT);

  // RGB
  pinMode(pinR, OUTPUT);
  pinMode(pinG, OUTPUT);
  pinMode(pinB, OUTPUT);

  // Ultrasónico
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Servo
  cabeza.attach(servoPin);
  cabeza.write(90);

  // Buzzer
  pinMode(buzzerPin, OUTPUT);

  Serial.println("Sistema listo con Motores + RGB + Servo + Ultrasonido + Buzzer");
}

// ======================== LOOP =========================
void loop() {
  
  if (BT.available()) {
    cmd = BT.read();
    Serial.print("Cmd recibido: ");
    Serial.println(cmd);

    switch (cmd) {

      // === MOTORES ===
      case 'A': adelante(); break;
      case 'B': atras(); break;
      case 'C': izquierda(); break;
      case 'D': derecha(); break;
      case 'E': stopMotores(); break;

      // === SERVO ===
      case 'F': cabeza.write(0); break;
      case 'G': cabeza.write(45); break;
      case 'H': cabeza.write(90); break;
      case 'I': cabeza.write(135); break;
      case 'J': cabeza.write(180); break;

      // === RGB ===
      case 'K': encenderColor(255,0,0); break;  // Rojo
      case 'L': encenderColor(0,255,0); break;  // Verde
      case 'M': encenderColor(0,0,255); break;  // Azul
      case 'N': encenderColor(255,255,0); break; // Amarillo
      case 'O': encenderColor(255,0,255); break; // Magenta
      case 'P': encenderColor(0,255,255); break; // Cian
      case 'Q': encenderColor(255,255,255); break; // Blanco
      case 'R': encenderColor(0,0,0); break; // Off

      // === ULTRASONIDO ===
      case 'S':
        Serial.print("Distancia: ");
        Serial.print(medirDistancia());
        Serial.println(" cm");
        break;

      // === BUZZER ===
      case 'T': beep(1000, 200); break;  // Tono corto
      case 'U': alarma(); break;        // Alarma continua
      case 'V': beep(500, 300); break;  // Grave
      case 'W': beep(2000, 300); break; // Agudo
      case 'X': apagarBuzzer(); break;  // Stop buzzer
    }
  }
}
