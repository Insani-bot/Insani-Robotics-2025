//Lógica para mover el robot por APP + Mod. Bluetooth

#include <SoftwareSerial.h>

SoftwareSerial BT(9, 8); // RX, TX

// ==== Pines del puente H ====
const int M1A = 4;
const int M1B = 2;

const int M2A = 12;
const int M2B = 7;

char dato = '\0';

void setup() {
  Serial.begin(9600);
  BT.begin(9600);

  // Configurar pines como salida
  pinMode(M1A, OUTPUT);
  pinMode(M1B, OUTPUT);
  pinMode(M2A, OUTPUT);
  pinMode(M2B, OUTPUT);

  // Detener motores al inicio
  detener();
}

// ===================== FUNCIONES DE MOVIMIENTO =====================

// Adelante: ambos motores hacia adelante
void adelante() {
  digitalWrite(M1A, HIGH);
  digitalWrite(M1B, LOW);
  digitalWrite(M2A, HIGH);
  digitalWrite(M2B, LOW);
}

// Atrás: ambos motores hacia atrás
void atras() {
  digitalWrite(M1A, LOW);
  digitalWrite(M1B, HIGH);
  digitalWrite(M2A, LOW);
  digitalWrite(M2B, HIGH);
}

// Derecha: motor izquierdo avanza, derecho retrocede o se detiene
void derecha() {
  digitalWrite(M1A, HIGH);
  digitalWrite(M1B, LOW);
  digitalWrite(M2A, LOW);
  digitalWrite(M2B, HIGH);
}

// Izquierda: motor derecho avanza, izquierdo retrocede o se detiene
void izquierda() {
  digitalWrite(M1A, LOW);
  digitalWrite(M1B, HIGH);
  digitalWrite(M2A, HIGH);
  digitalWrite(M2B, LOW);
}

// Stop
void detener() {
  digitalWrite(M1A, LOW);
  digitalWrite(M1B, LOW);
  digitalWrite(M2A, LOW);
  digitalWrite(M2B, LOW);
}

// ============================ LOOP ============================

void loop() {
  if (BT.available()) {
    dato = BT.read();
    Serial.print("Recibido: ");
    Serial.println(dato);

    if (dato == 'F') {
      Serial.println("ADELANTE");
      adelante();
    }
    else if (dato == 'B') {
      Serial.println("ATRÁS");
      atras();
    }
    else if (dato == 'R') {
      Serial.println("DERECHA");
      derecha();
    }
    else if (dato == 'L') {
      Serial.println("IZQUIERDA");
      izquierda();
    }
    else if (dato == 'x') { 
      Serial.println("DETENER");
      detener();
    }
  }
}
