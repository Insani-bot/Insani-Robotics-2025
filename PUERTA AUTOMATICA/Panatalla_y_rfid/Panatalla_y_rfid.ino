#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP32Servo.h>

Servo cerrojo;  // create servo object to control a servo
int pos = 90;    // variable to store the servo position
int servoPin = 25;

#define SS_PIN  5
#define RST_PIN 4

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
MFRC522 mfrc522(SS_PIN, RST_PIN);

// UIDs Autorizados
String UIDs_Autorizados[] = {
  "72B7973F",         // key 1
  "33E648DA",         // key 2
  "0452745DBB2A81"    // key 3
};
//Asociados a:
String nombres[] = {
  "JUAN_key",
  "DIANA_key",
  "MaElisa_key"
};


char str[32] = "";
String UID_Result = "";

unsigned long lastReadTime = 0;
unsigned long debounceDelay = 3000;

void setup() {
  Serial.begin(115200);
  SPI.begin();
  mfrc522.PCD_Init();

  // Allow allocation of all timers
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  cerrojo.setPeriodHertz(60);
  cerrojo.attach(servoPin, 400, 2000);
  cerrojo.write(90);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("Pantalla OLED no encontrada");
    while (true);
  }
  display.display();
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);

  display.display();

  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(2);
  display.print("ACERQUE-->");
  display.setCursor(0, 16);
  display.println("LLAVE ");
  display.display();

  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  analogWrite(12, 0);
  analogWrite(13, 0);

}

void loop() {
  if (millis() - lastReadTime < debounceDelay) return;

  if (!mfrc522.PICC_IsNewCardPresent()) return;
  if (!mfrc522.PICC_ReadCardSerial()) return;

  byteArray_to_string(mfrc522.uid.uidByte, mfrc522.uid.size, str);
  UID_Result = str;

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
  lastReadTime = millis();

  mostrarResultado(UID_Result);

  delay(100);

  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(2);
  display.print("ACERQUE-->");
  display.setCursor(0, 16);
  display.println("LLAVE ");
  display.display();
}

void mostrarResultado(String uid) {
  String nombre = obtenerNombrePorUID(uid);

  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.print("Llave: ");
  display.println(nombre);
  display.setCursor(0, 16);
  display.setTextSize(2);
  Serial.print("UID: ");
  Serial.println(uid);


  if (nombre != "") {
    display.println("Bienvenido ");
    analogWrite(12, 255);
    analogWrite(13, 0);
    display.display();

    for (pos = 90; pos >= 0; pos -= 2) { // goes from 180 degrees to 0 degrees
      cerrojo.write(pos);    // tell servo to go to position in variable 'pos'
      delay(15);             // waits 15ms for the servo to reach the position
    }
    delay(3000);
    for (pos = 0; pos <= 90; pos += 2) { // goes from 0 degrees to 180 degrees
      // in steps of 1 degree
      cerrojo.write(pos);    // tell servo to go to position in variable 'pos'
      delay(15);             // waits 15ms for the servo to reach the position
    }
    analogWrite(12, 0);     // Apagar LEDs
    analogWrite(13, 0);
  } else {
    display.println("NO ACCESO");
    analogWrite(12, 0);
    analogWrite(13, 100);
    display.display();
    delay(1000);
    analogWrite(12, 0);     // Apagar LEDs
    analogWrite(13, 0);
  }


  //display.display();
}

String obtenerNombrePorUID(String uid) {
  for (int i = 0; i < sizeof(UIDs_Autorizados) / sizeof(UIDs_Autorizados[0]); i++) {
    if (uid == UIDs_Autorizados[i]) {
      return nombres[i];
    }
  }
  return "";
}

void byteArray_to_string(byte array[], unsigned int len, char buffer[]) {
  for (unsigned int i = 0; i < len; i++) {
    byte nib1 = (array[i] >> 4) & 0x0F;
    byte nib2 = (array[i] >> 0) & 0x0F;
    buffer[i * 2 + 0] = nib1  < 0xA ? '0' + nib1  : 'A' + nib1  - 0xA;
    buffer[i * 2 + 1] = nib2  < 0xA ? '0' + nib2  : 'A' + nib2  - 0xA;
  }
  buffer[len * 2] = '\0';
}
