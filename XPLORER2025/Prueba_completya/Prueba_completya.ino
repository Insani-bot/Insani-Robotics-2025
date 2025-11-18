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
const int pinB = 3; // también usado para servo en tu otro diseño, aquí solo RGB

// === ULTRASONIDO ===
const int trigPin = 13;
const int echoPin = 10;

// === SERVO CABEZA ===
Servo cabeza;
const int servoPin = 5; // <-- si estás usando el servo, quita el LED azul en este pin

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

  Serial.println("Sistema listo para pruebas Bluetooth!");
}

// ======================== LOOP =========================
void loop() {
  
  if (BT.available()) {
    cmd = BT.read();
    Serial.print("Cmd recibido: ");
    Serial.println(cmd);

    switch (cmd) {

      // === MOTORES ===
      case 'A': Serial.println("Motor: ADELANTE"); adelante(); break;
      case 'B': Serial.println("Motor: ATRAS"); atras(); break;
      case 'C': Serial.println("Motor: IZQUIERDA"); izquierda(); break;
      case 'D': Serial.println("Motor: DERECHA"); derecha(); break;
      case 'E': Serial.println("Motor: STOP"); stopMotores(); break;

      // === SERVO ===
      case 'F': cabeza.write(0);   Serial.println("Servo: 0°");   break;
      case 'G': cabeza.write(45);  Serial.println("Servo: 45°");  break;
      case 'H': cabeza.write(90);  Serial.println("Servo: 90°");  break;
      case 'I': cabeza.write(135); Serial.println("Servo: 135°"); break;
      case 'J': cabeza.write(180); Serial.println("Servo: 180°"); break;

      // === RGB LED ===
      case 'K': encenderColor(255,0,0);   Serial.println("Color: ROJO");   break;
      case 'L': encenderColor(0,255,0);   Serial.println("Color: VERDE");  break;
      case 'M': encenderColor(0,0,255);   Serial.println("Color: AZUL");   break;
      case 'N': encenderColor(255,255,0); Serial.println("Color: AMARILLO"); break;
      case 'O': encenderColor(255,0,255); Serial.println("Color: MAGENTA"); break;
      case 'P': encenderColor(0,255,255); Serial.println("Color: CIAN"); break;
      case 'Q': encenderColor(255,255,255); Serial.println("Color: BLANCO"); break;
      case 'R': encenderColor(0,0,0);     Serial.println("Color: APAGADO"); break;

      // === SENSOR ULTRASONICO ===
      case 'S':
        Serial.println("Leyendo distancia...");
        Serial.print("Distancia: ");
        Serial.print(medirDistancia());
        Serial.println(" cm");
        break;
    }
  }
}
