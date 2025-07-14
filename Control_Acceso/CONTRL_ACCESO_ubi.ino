#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "UbidotsEsp32Mqtt.h"

// Pines RFID y pantalla
#define SS_PIN  5
#define RST_PIN 4
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
MFRC522 mfrc522(SS_PIN, RST_PIN);

// UIDs autorizados y nombres
String UIDs_Autorizados[] = {
  "72B7973F",         // Natasha
  "33E648DA",         // Steve
  "0452745DBB2A81"    // Tony
};

String nombres[] = {
  "Natasha",
  "Steve",
  "Tony"
};

// Ubidots CREDENCIALES
const char *UBIDOTS_TOKEN = "BBUS-OhbibPfX5Gv7Ou8ARDBnnEY2D8knSx";  // Put here your Ubidots TOKEN
const char *WIFI_SSID = "NettPlus_Elena Poma";
const char *WIFI_PASS = "Elena12345";
const char *DEVICE_LABEL = "ACCESO";
const char *VARIABLE_LABEL1 = "llave1";
const char *VARIABLE_LABEL2 = "llave2";
const char *VARIABLE_LABEL3 = "llave3";
const char *LED_VARIABLE_LABEL = "rele";

Ubidots ubidots(UBIDOTS_TOKEN);

// Variables generales
char str[32] = "";
String UID_Result = "";
unsigned long lastReadTime = 0;
unsigned long debounceDelay = 3000;


void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
    if ((char)payload[0] == '1')
    {
      digitalWrite(2, HIGH);
    }
    else
    {
      digitalWrite(2, LOW);
    }
  }
  Serial.println();
}



void setup() {
  Serial.begin(115200);
  SPI.begin();
  mfrc522.PCD_Init();

  // OLED 0.91"
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("Pantalla OLED no encontrada");
    while (true);
  }

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.display();
  display.setCursor(0, 0);
  display.print("ACERQUE-->");
  display.setCursor(0, 16);
  display.println("LLAVE ");
  display.display();

  // Pines
  pinMode(12, OUTPUT);  // LED verde
  pinMode(13, OUTPUT);  // LED rojo
  pinMode(2, OUTPUT);   // Pin control remoto desde Ubidots (RELÉ)

  digitalWrite(2, HIGH);
  delay(500);
  digitalWrite(2, LOW);

  digitalWrite(12, LOW);
  digitalWrite(13, LOW);

  // Ubidots
  ubidots.connectToWifi(WIFI_SSID, WIFI_PASS);
  ubidots.setCallback(callback);
  ubidots.setup();
  ubidots.reconnect();
  ubidots.subscribeLastValue(DEVICE_LABEL, LED_VARIABLE_LABEL);
}

void loop() {
  ubidots.loop();
  if (!ubidots.connected()) {
    ubidots.reconnect();
    ubidots.subscribeLastValue(DEVICE_LABEL, LED_VARIABLE_LABEL);
  }

  if (millis() - lastReadTime < debounceDelay) return;

  if (!mfrc522.PICC_IsNewCardPresent()) return;
  if (!mfrc522.PICC_ReadCardSerial()) return;

  byteArray_to_string(mfrc522.uid.uidByte, mfrc522.uid.size, str);
  UID_Result = str;

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
  lastReadTime = millis();

  mostrarResultado(UID_Result);

  delay(2000);

  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(2);
  display.print("ACERQUE-->");
  display.setCursor(0, 16);
  display.println("LLAVE ");
  display.display();

  // Apagar LEDs luego de mostrar
  digitalWrite(12, LOW);
  digitalWrite(13, LOW);
}

void mostrarResultado(String uid) {
  String nombre = obtenerNombrePorUID(uid);
  int indice = obtenerIndicePorUID(uid);

  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.print("LLAVE: ");
  display.println(nombre);
  display.setCursor(0, 16);
  display.setTextSize(2);
  Serial.print("UID: ");
  Serial.println(uid);

  if (nombre != "") {
    digitalWrite(12, HIGH);  // LED azul
    digitalWrite(13, LOW);
    display.println("Bienvenido");
  } else {
    digitalWrite(12, LOW);
    digitalWrite(13, HIGH);  // LED rojo
    display.println("NO ACCESO");
  }

  display.display();

  // Envío a Ubidots
  ubidots.add(VARIABLE_LABEL1, (indice == 0) ? 1 : 0);
  ubidots.add(VARIABLE_LABEL2, (indice == 1) ? 1 : 0);
  ubidots.add(VARIABLE_LABEL3, (indice == 2) ? 1 : 0);

  ubidots.publish(DEVICE_LABEL);
}

String obtenerNombrePorUID(String uid) {
  for (int i = 0; i < sizeof(UIDs_Autorizados) / sizeof(UIDs_Autorizados[0]); i++) {
    if (uid == UIDs_Autorizados[i]) {
      return nombres[i];
    }
  }
  return "";
}

int obtenerIndicePorUID(String uid) {
  for (int i = 0; i < sizeof(UIDs_Autorizados) / sizeof(UIDs_Autorizados[0]); i++) {
    if (uid == UIDs_Autorizados[i]) {
      return i;
    }
  }
  return -1;
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
