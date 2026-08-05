#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// -------- OLED --------
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// -------- MOTOR --------
const int EN = 23;
const int IN1 = 32;
const int IN2 = 33;

const int finPlegado = 25;
const int finDesplegado = 26;

// -------- SENSORES --------
const int pinLluvia = 35;

// -------- PWM --------
const int canalPWM = 0;
const int frecuencia = 5000;
const int resolucion = 8;
const int velocidadMotor = 235;

// -------- ESTADOS --------
enum Estado { PLEGADO, DESPLEGADO, MOVIENDO_A_DESPLEGAR, MOVIENDO_A_PLEGAR };
Estado estadoActual;

unsigned long tiempoMovimiento = 0;
const unsigned long tiempoMaximo = 10000;

unsigned long tiempoLectura = 0;
const unsigned long intervaloLectura = 300; // Lectura rápida para responder casi al instante

unsigned long tiempoOLED = 0;
const unsigned long intervaloOLED = 300;

float lluviaPct = 0;
String clima = "SECO";

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

  // PWM (ESP32 core v2.x)
  ledcSetup(canalPWM, frecuencia, resolucion);
  ledcAttachPin(EN, canalPWM);

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

  // -------- LECTURA SENSOR Y DECISIÓN --------
  if (ahora - tiempoLectura >= intervaloLectura) {
    tiempoLectura = ahora;

    int lluviaRaw = leerPromedio(pinLluvia);

    // Mapeo directo acorde a tu módulo
    lluviaPct = (lluviaRaw / 4095.0) * 100.0;

    bool cubrir = false;
    bool descubrir = false;

    // Umbrales de alta sensibilidad
    if (lluviaPct > 2.0) {       // Activa con las primeras gotas
      clima = "LLUVIA";
      cubrir = true;
    } 
    else if (lluviaPct < 0.8) {  // Repliega solo al secarse completamente
      clima = "SECO";
      descubrir = true;
    }

    // Evaluación de Máquina de Estados
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

      default:
        break;
    }
  }

  // -------- LÓGICA CONTINUA DE PARADA MOTOR --------
  if (estadoActual == MOVIENDO_A_DESPLEGAR) {
    if (digitalRead(finDesplegado) == LOW || ahora - tiempoMovimiento > tiempoMaximo) {
      detenerMotor();
      estadoActual = DESPLEGADO;
    }
  }
  else if (estadoActual == MOVIENDO_A_PLEGAR) {
    if (digitalRead(finPlegado) == LOW || ahora - tiempoMovimiento > tiempoMaximo) {
      detenerMotor();
      estadoActual = PLEGADO;
    }
  }

  // -------- ACTUALIZACIÓN OLED --------
  if (ahora - tiempoOLED >= intervaloOLED) {
    tiempoOLED = ahora;

    display.clearDisplay();
    display.setTextColor(WHITE);

    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print("LLUVIA: ");
    display.print(lluviaPct, 1); // 1 decimal para apreciar cambios finos
    display.print("%");

    display.setCursor(0, 16);
    display.print("ESTADO: ");
    if (estadoActual == PLEGADO) display.print("DESCUBIERTO");
    else if (estadoActual == DESPLEGADO) display.print("CUBIERTO");
    else display.print("MOVIENDO");

    // TEXTO CLIMA CENTRADO
    display.setTextSize(2);

    int16_t x1, y1;
    uint16_t w, h;
    display.getTextBounds(clima, 0, 0, &x1, &y1, &w, &h);
    int x = (128 - w) / 2;

    display.setCursor(x, 42);
    display.print(clima);

    display.display();
  }
}