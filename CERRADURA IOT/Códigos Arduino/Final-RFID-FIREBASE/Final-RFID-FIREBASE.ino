#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <time.h>
#include <ESP32Servo.h>

// ================== CONFIG WIFI Y FIREBASE ==================
const char* WIFI_SSID = "Tu insani wifi "; //Cisco-ISDTAB
const char* WIFI_PASSWORD = "la contraseña";//L4B0R4ToR10_c1sc0.2024
//R-ISTDAB-ESTUDIANTES
//T3cn0l0g1c0
#define API_KEY "AIzaSyBRjLvZXhYDIDReDrYwINRXNKiUJuECYyA"
#define PROJECT_ID "registro-inlock"
#define USER_EMAIL "inlock@gmail.com"
#define USER_PASSWORD "1234abcd"

// ================== CONFIG RFID ==================
#define SS_PIN 5
#define RST_PIN 4
MFRC522 mfrc522(SS_PIN, RST_PIN);
char str[32] = "";
String UID_Result = "";

// ================== CONFIG OLED ==================
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// ================== FIREBASE VARS ==================
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// ================== DEBOUNCE ==================
unsigned long lastReadTime = 0;
unsigned long debounceDelay = 3000;

// ================== SERVO ==================
Servo cerrojo;
int pos = 90;
int servoPin = 25;

// ================== FUNCIONES ==================

// Convertir UID a string
void byteArray_to_string(byte array[], unsigned int len, char buffer[]) {
  for (unsigned int i = 0; i < len; i++) {
    byte nib1 = (array[i] >> 4) & 0x0F;
    byte nib2 = (array[i] >> 0) & 0x0F;
    buffer[i * 2 + 0] = nib1 < 0xA ? '0' + nib1 : 'A' + nib1 - 0xA;
    buffer[i * 2 + 1] = nib2 < 0xA ? '0' + nib2 : 'A' + nib2 - 0xA;
  }
  buffer[len * 2] = '\0';
}

// Consultar Firestore: administrador/UID
bool verificarAccesoEnFirebase(String uid, String& nombre) {
  String documentPath = "administrador/" + uid;

  if (Firebase.Firestore.getDocument(&fbdo, PROJECT_ID, "", documentPath.c_str())) {
    FirebaseJson json;
    json.setJsonData(fbdo.payload().c_str());

    FirebaseJsonData estadoData;
    FirebaseJsonData nombreData;

    json.get(estadoData, "fields/Estado/integerValue");
    json.get(nombreData, "fields/Nombre/stringValue");

    int estado = estadoData.intValue;
    nombre = nombreData.stringValue.c_str();

    if (estado == 1) {
      return true;  // acceso permitido
    } else {
      return false;  // acceso denegado
    }
  } else {
    Serial.println("Error leyendo documento: " + fbdo.errorReason());
    return false;
  }
}

// Registrar acceso en Firestore
bool registrarAccesoRFID(const String& uid, const String& nombre) {
  time_t now = time(nullptr);
  struct tm* ti = localtime(&now);

  char fecha[11];
  strftime(fecha, sizeof(fecha), "%Y-%m-%d", ti);
  char hora[6];
  strftime(hora, sizeof(hora), "%H:%M", ti);

  FirebaseJson json;
  json.set("fields/nombre/stringValue", nombre);
  json.set("fields/uid/stringValue", uid);
  json.set("fields/fecha/stringValue", String(fecha));
  json.set("fields/hora/stringValue", String(hora));

  String docId = uid + "_" + String(fecha) + "_" + String(hora);
  docId.replace(":", "-");

  bool ok = Firebase.Firestore.createDocument(&fbdo, PROJECT_ID, "", ("accesos/" + docId).c_str(), json.raw());

  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  if (ok) {
    display.println("Registro OK");
    display.println(nombre);
    display.display();
  } else {
    display.println("Error guardando");
    display.display();
  }

  return ok;
}


// ================== SETUP ==================
void setup() {
  Serial.begin(115200);
  SPI.begin();
  mfrc522.PCD_Init();

  // Servo
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  cerrojo.setPeriodHertz(60);
  cerrojo.attach(servoPin, 400, 2000);
  cerrojo.write(90);

  // OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("Pantalla OLED no encontrada");
    while (true)
      ;
  }
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("Iniciando...");
  display.display();
  delay(200);  // Esperar para que se vea

  // WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.println("Conectando WiFi...");
  display.display();
  while (WiFi.status() != WL_CONNECTED) {
    delay(400);
    Serial.print(".");
  }
  display.println("WiFi OK");
  display.display();
  delay(200);

  // Hora NTP
  configTime(-5 * 3600, 0, "pool.ntp.org", "time.nist.gov");
  display.println("Hora OK");
  display.display();
  delay(200);

  // Firebase
  config.api_key = API_KEY;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  display.println("Firebase OK");
  display.display();
  delay(1000);

  // Mensaje inicial
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.print("ACERQUE-->");
  display.setCursor(0, 16);
  display.println("LLAVE ");
  display.display();
  Serial.println("Listos!");
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  digitalWrite(12, 0);
  digitalWrite(13, 0);
}

// ================== LOOP ==================
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

  // Mensaje inicial
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.print("ACERQUE-->");
  display.setCursor(0, 16);
  display.println("LLAVE ");
  display.display();
}

// Mostrar resultado acceso
void mostrarResultado(String uid) {
  String nombre;

  if (verificarAccesoEnFirebase(uid, nombre)) {
    registrarAccesoRFID(uid, nombre);

    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.print("Llave: ");
    display.println(nombre);
    display.setCursor(0, 16);
    display.setTextSize(2);
    display.print("BIENVENIDO ");
    digitalWrite(12, 1);
    digitalWrite(13, 0);
    display.display();

    for (pos = 90; pos >= 0; pos -= 2) {
      cerrojo.write(pos);
      delay(15);
    }
    delay(3000);
    for (pos = 0; pos <= 90; pos += 2) {
      cerrojo.write(pos);
      delay(15);
    }

  } else {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(2);
    display.println("NO ACCESO");
    digitalWrite(12, 0);
    digitalWrite(13, 1);
    display.display();
  }
  delay(1000);
  digitalWrite(12, 0);
  digitalWrite(13, 0);
}
