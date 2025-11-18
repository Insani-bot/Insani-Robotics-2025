#include <SoftwareSerial.h>

SoftwareSerial BT(9, 8); // RX, TX

char dato = '\0';

void setup() {
  Serial.begin(9600);
  BT.begin(9600);
}

void loop() {
  if (BT.available()) {
    dato = BT.read();

    if (dato == 'A') {
      Serial.println("ADELANTE!!");
    }
    else if (dato == 'B') {
      Serial.println("ATRAS!!");
    }
    else if (dato == 'C') {
      Serial.println("DERECHA!!");
    }
    else if (dato == 'D') {
      Serial.println("IZQUIERDA!!");
    }

    Serial.println(dato);
  }
}
