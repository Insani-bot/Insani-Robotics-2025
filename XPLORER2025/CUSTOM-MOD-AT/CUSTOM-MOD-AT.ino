#include <SoftwareSerial.h>
SoftwareSerial BT(9, 8); // RX, TX

void setup() {
  Serial.begin(9600);
  BT.begin(38400); // Velocidad AT fija para HC-05 en modo AT
  Serial.println("Modo AT listo");
}

void loop() {
  if (BT.available()) {
    Serial.write(BT.read());
  }
  if (Serial.available()) {
    BT.write(Serial.read());
  }
}
