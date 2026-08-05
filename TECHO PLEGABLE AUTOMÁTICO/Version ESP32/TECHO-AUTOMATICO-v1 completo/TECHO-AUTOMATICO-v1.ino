#include <DHT.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// -------- OLED --------
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// -------- DHT --------
#define DHTPIN 19
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// -------- MOTOR --------
const int EN = 23;
const int IN1 = 32;
const int IN2 = 33;

const int finPlegado = 25;
const int finDesplegado = 26;

// -------- SENSORES --------
const int pinLDR = 34;
const int pinLluvia = 35;

// -------- PWM --------
const int canalPWM = 0;
const int frecuencia = 5000;
const int resolucion = 8;
const int velocidadMotor = 240;

// -------- ESTADOS --------
enum Estado { PLEGADO, DESPLEGADO, MOVIENDO_A_DESPLEGAR, MOVIENDO_A_PLEGAR };
Estado estadoActual;

unsigned long tiempoMovimiento = 0;
const unsigned long tiempoMaximo = 10000;

unsigned long tiempoLectura = 0;
const unsigned long intervalo = 800;

float humedad = 0;
float temperatura = 0;
float luzPct = 0;
float lluviaPct = 0;

String clima = "NUBLADO";

// -------- FUNCIONES --------

void moverDesplegar() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  ledcWrite(canalPWM, velocidadMotor);
}

void moverPlegar() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  ledcWrite(canalPWM, velocidadMotor);
}

void detenerMotor() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  ledcWrite(canalPWM, 0);
}

int leerPromedio(int pin) {
  long suma = 0;
  for (int i = 0; i < 8; i++) {
    suma += analogRead(pin);
  }
  return suma / 8;
}

// -------- SETUP --------

void setup() {

  Serial.begin(115200);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(finPlegado, INPUT_PULLUP);
  pinMode(finDesplegado, INPUT_PULLUP);

  ledcSetup(canalPWM, frecuencia, resolucion);
  ledcAttachPin(EN, canalPWM);

  dht.begin();

  Wire.begin(21, 22);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  if (digitalRead(finPlegado) == LOW)
    estadoActual = PLEGADO;
  else
    estadoActual = DESPLEGADO;
}

// -------- LOOP --------

void loop() {

  unsigned long ahora = millis();

  // -------- LECTURA SENSORES --------
  if (ahora - tiempoLectura >= intervalo) {

    tiempoLectura = ahora;

    humedad = dht.readHumidity();
    temperatura = dht.readTemperature();

    if (isnan(humedad)) humedad = 0;
    if (isnan(temperatura)) temperatura = 0;

    int lluviaRaw = leerPromedio(pinLluvia);
    int luzRaw = leerPromedio(pinLDR);

    lluviaPct = (lluviaRaw / 4095.0) * 100.0;
    luzPct = 100.0 - ((luzRaw / 4095.0) * 100.0);
  }

  // -------- CLIMA Y DECISION --------

  bool lluviaFuerte = (lluviaPct > 10);
  bool alertaLluvia = (humedad > 60 && luzPct < 45);
  bool buenClima = (lluviaPct < 3 && luzPct > 65);

  bool cubrir = false;
  bool descubrir = false;

  if (lluviaFuerte) {
    clima = "LLUVIA";
    cubrir = true;
  }
  else if (alertaLluvia) {
    clima = "ALERTA";
    cubrir = true;
  }
  else if (buenClima) {
    clima = "SOLEADO";
    descubrir = true;
  }
  else {
    clima = "NUBLADO";
  }

  // -------- MAQUINA DE ESTADOS --------

  switch (estadoActual) {

    case PLEGADO:
      if (cubrir) {
        moverDesplegar();
        tiempoMovimiento = ahora;
        estadoActual = MOVIENDO_A_DESPLEGAR;
      }
      break;

    case DESPLEGADO:
      if (descubrir) {
        moverPlegar();
        tiempoMovimiento = ahora;
        estadoActual = MOVIENDO_A_PLEGAR;
      }
      break;

    case MOVIENDO_A_DESPLEGAR:
      if (digitalRead(finDesplegado) == LOW || ahora - tiempoMovimiento > tiempoMaximo) {
        detenerMotor();
        estadoActual = DESPLEGADO;
      }
      break;

    case MOVIENDO_A_PLEGAR:
      if (digitalRead(finPlegado) == LOW || ahora - tiempoMovimiento > tiempoMaximo) {
        detenerMotor();
        estadoActual = PLEGADO;
      }
      break;
  }

  // -------- OLED --------

  display.clearDisplay();
  display.setTextColor(WHITE);

  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("H:");
  display.print(humedad, 0);
  display.print("% T:");
  display.print(temperatura, 0);

  display.setCursor(0, 12);
  display.print("L:");
  display.print(luzPct, 0);
  display.print("% R:");
  display.print(lluviaPct, 0);

  display.setCursor(0, 24);
  
  display.print("ESTADO: ");
  if (estadoActual == PLEGADO) display.print("DESCUBIERTO");
  else if (estadoActual == DESPLEGADO) display.print("CUBIERTO");
  else display.print("MOVIENDO");

  // CLIMA GRANDE
  display.setTextSize(2);

  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(clima, 0, 0, &x1, &y1, &w, &h);
  int x = (128 - w) / 2;

  display.setCursor(x, 45);
  display.print(clima);

  display.display();
}