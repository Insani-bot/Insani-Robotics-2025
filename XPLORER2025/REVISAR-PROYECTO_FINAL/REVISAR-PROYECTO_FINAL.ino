#include <Servo.h>

// === MOTORES H-BRIDGE ===
const int M1A = 4;
const int M1B = 2;
const int M2A = 12;
const int M2B = 7;

// === ULTRASONIDO ===
const int trigPin = 13;
const int echoPin = 10;

// === SERVO CABEZA ===
Servo cabeza;
const int pinServo = 11;   // <-- AJÚSTALO AL TUYO

// === FUNCION MEDIR DISTANCIA ===
long medirDistancia() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(3);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duracion = pulseIn(echoPin, HIGH, 20000);
  long distancia = duracion * 0.034 / 2;

  return distancia;
}

// === MOVIMIENTOS DEL ROBOT ===
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

void parar() {
  digitalWrite(M1A, LOW);
  digitalWrite(M1B, LOW);
  digitalWrite(M2A, LOW);
  digitalWrite(M2B, LOW);
}

// =====================================================
void setup() {
  Serial.begin(9600);

  pinMode(M1A, OUTPUT);
  pinMode(M1B, OUTPUT);
  pinMode(M2A, OUTPUT);
  pinMode(M2B, OUTPUT);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  cabeza.attach(pinServo);
  cabeza.write(90);   // Frente
  delay(500);
}

// =====================================================
void loop() {
  long distFrente = medirDistancia();
  Serial.print("Frente: "); Serial.println(distFrente);

  // Si TODO está despejado, avanzar
  if (distFrente > 20) {
    adelante();
  }
  else {
    // Freno de emergencia
    parar();
    delay(300);

    // Buscar LADO IZQUIERDO
    cabeza.write(150);
    delay(500);
    long distIzq = medirDistancia();
    Serial.print("Izquierda: "); Serial.println(distIzq);

    // Buscar LADO DERECHO
    cabeza.write(30);
    delay(500);
    long distDer = medirDistancia();
    Serial.print("Derecha: "); Serial.println(distDer);

    // Volver al centro
    cabeza.write(90);
    delay(300);

    // Elegir camino mayor distancia
    if (distIzq > distDer) {
      Serial.println("Girando a la izquierda!");
      izquierda();
      delay(500);
    } else {
      Serial.println("Girando a la derecha!");
      derecha();
      delay(500);
    }
  }

  delay(80);
}
