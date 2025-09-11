#include <WiFi.h>
#include <Firebase_ESP_Client.h>

// ================== CAMBIA CONFIG WIFI Y FIREBASE ==================
const char* WIFI_SSID = "Tu WIFI INSANI";       //  Cambia por tu WiFi
const char* WIFI_PASSWORD = "TU CLAVE INSANI";  //  Cambia por tu clave
#define API_KEY "AwzaSyBRjLvZXhYDIDRedrYwINROONKiUJuECYyA"  // API Key Firebase
#define FIREBASE_PROJECT_ID "registro-inlock"              // ID del proyecto
#define USER_EMAIL "inlock@gmail.com"                      // Usuario con permisos
#define USER_PASSWORD "Insani4abcd"                           // Password del usuario

// ================== CAMBIA POR TUS UIDs ==================
String uids[] = {
  "0432A1A81",
  "04C8FE521",
  "04519A581",
  "044ABD2A1",
  "0D0454D3"
};
// ================== CAMBIA POR SUS NOMBRES ==================
String Llaves[] = {
  "Jose",
  "Maria",
  "Pedro",
  "Luisa",
  "Carlos"
};

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// ========================
// CONEXIÓN WIFI
// ========================
void conectarWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Conectando a WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" Conectado!");
}

// ========================
// CONEXIÓN FIREBASE
// ========================
void conectarFirebase() {
  config.api_key = API_KEY;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

// ========================
// CREAR DOCUMENTOS
// ========================
void crearUsuarios() {
  for (int i = 0; i < 5; i++) {
    String docPath = "usuarios/" + uids[i];  // UID como ID de documento

    FirebaseJson content;
    content.set("fields/Nombre/stringValue", Llaves[i]);
    content.set("fields/UID/stringValue", uids[i]);
    content.set("fields/Estado/integerValue", 1);

    Serial.print("Creando usuario: ");
    Serial.println(Llaves[i]);

    if (Firebase.Firestore.createDocument(&fbdo, FIREBASE_PROJECT_ID, "", docPath.c_str(), content.raw())) {
      Serial.println("Usuario creado correctamente");
    } else {
      Serial.println(" Error: " + fbdo.errorReason());
    }
  }
}

void setup() {
  Serial.begin(115200);
  conectarWiFi();
  conectarFirebase();
  delay(2000);
  crearUsuarios();  //  Crea todos los usuarios en Firestore
}

void loop() {
  // No hacemos nada en loop
}
