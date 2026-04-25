// -------- HW-038--------
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// -------- OLED --------
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// -------- MOTOR --------
const int EN = 3;
const int IN1 = 8;
const int IN2 = 9;

const int finDesplegado = 6;
const int finPlegado = 5;

// -------- SENSOR --------
const int pinLluvia = A1;

// -------- CONFIG --------
const int velocidadMotor = 240;

// -------- CALIBRACION REAL 1023--------
int valorSeco = 580;
int valorMojado = 20;

// -------- HISTÉRESIS --------
const int UMBRAL_LLUVIA = 60;
const int UMBRAL_SECO   = 40;

// -------- ESTADOS --------
enum Estado { 
  PLEGADO, 
  DESPLEGADO, 
  MOVIENDO_A_DESPLEGAR, 
  MOVIENDO_A_PLEGAR 
};

Estado estadoActual;

// -------- TIEMPOS --------
unsigned long tiempoMovimiento = 0;
const unsigned long tiempoMaximo = 10000;

unsigned long tiempoLectura = 0;
const unsigned long intervalo = 500;

// -------- VARIABLES --------
float lluviaPct = 0;
String clima = "NUBLADO";

// -------- FUNCIONES --------

void moverDesplegar() {
  // CUBRIR
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  analogWrite(EN, velocidadMotor);
}

void moverPlegar() {
  // DESCUBRIR
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(EN, velocidadMotor);
}

void detenerMotor() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  analogWrite(EN, 0);
}

int leerPromedio(int pin) {
  long suma = 0;
  for (int i = 0; i < 5; i++) {
    suma += analogRead(pin);
  }
  return suma / 5;
}

// -------- SETUP --------

void setup() {

  Serial.begin(9600);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(EN, OUTPUT);

  pinMode(finPlegado, INPUT_PULLUP);
  pinMode(finDesplegado, INPUT_PULLUP);

  Wire.begin();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  display.clearDisplay();
  display.display();

  // -------- ESTADO INICIAL REAL --------
  if (digitalRead(finPlegado) == LOW) {
    estadoActual = PLEGADO;
  }
  else if (digitalRead(finDesplegado) == LOW) {
    estadoActual = DESPLEGADO;
  }
  else {
    // seguridad → plegar
    moverPlegar();
    tiempoMovimiento = millis();
    estadoActual = MOVIENDO_A_PLEGAR;
  }
}

// -------- LOOP --------

void loop() {

  unsigned long ahora = millis();

  // -------- LECTURA SENSOR --------
  if (ahora - tiempoLectura >= intervalo) {

    tiempoLectura = ahora;

    int lluviaRaw = leerPromedio(pinLluvia);

    lluviaPct = map(lluviaRaw, valorMojado, valorSeco, 0, 100);
    lluviaPct = constrain(lluviaPct, 0, 100);

    Serial.print("RAW: ");
    Serial.print(lluviaRaw);
    Serial.print(" %: ");
    Serial.println(lluviaPct);
  }

  // -------- HISTÉRESIS --------
  static bool estadoLluvia = false;

  if (!estadoLluvia && lluviaPct > UMBRAL_LLUVIA) {
    estadoLluvia = true;
  } 
  else if (estadoLluvia && lluviaPct < UMBRAL_SECO) {
    estadoLluvia = false;
  }

  bool cubrir = estadoLluvia;
  bool descubrir = !estadoLluvia;

  // -------- CLIMA --------
  if (estadoLluvia) clima = "LLUVIA";
  else if (lluviaPct < 20) clima = "SECO";
  else clima = "NUBLADO";

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
      if (digitalRead(finDesplegado) == LOW || 
          ahora - tiempoMovimiento > tiempoMaximo) {
        detenerMotor();
        estadoActual = DESPLEGADO;
      }
      break;

    case MOVIENDO_A_PLEGAR:
      if (digitalRead(finPlegado) == LOW || 
          ahora - tiempoMovimiento > tiempoMaximo) {
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
  display.print("LLUVIA: ");
  display.print(lluviaPct, 0);
  display.print("%");

  display.setCursor(0, 18);
  display.print("ESTADO: ");

  if (estadoActual == PLEGADO) display.print("DESCUBIERTO");
  else if (estadoActual == DESPLEGADO) display.print("CUBIERTO");
  else display.print("MOVIENDO");

  display.setTextSize(2);

  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(clima, 0, 0, &x1, &y1, &w, &h);
  int x = (128 - w) / 2;

  display.setCursor(x, 40);
  display.print(clima);

  display.display();
}