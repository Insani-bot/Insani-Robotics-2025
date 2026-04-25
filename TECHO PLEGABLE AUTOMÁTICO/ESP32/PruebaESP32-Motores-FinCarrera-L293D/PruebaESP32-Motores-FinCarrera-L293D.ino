const int EN = 23;
const int IN1 = 33;
const int IN2 = 32;

const int finPlegado = 25;
const int finDesplegado = 26;

const int canalPWM = 0;
const int frecuencia = 5000;
const int resolucion = 8;
const int velocidadMotor = 220;

enum Estado { PLEGADO, DESPLEGADO, MOVIENDO_A_DESPLEGAR, MOVIENDO_A_PLEGAR };
Estado estadoActual;

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

void setup() {
  Serial.begin(115200);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(finPlegado, INPUT_PULLUP);
  pinMode(finDesplegado, INPUT_PULLUP);

  ledcSetup(canalPWM, frecuencia, resolucion);
  ledcAttachPin(EN, canalPWM);

  Serial.println("TEST IDA Y VUELTA");

  estadoActual = MOVIENDO_A_DESPLEGAR;
  moverDesplegar();
}

void loop() {

  if (estadoActual == MOVIENDO_A_DESPLEGAR) {

    if (digitalRead(finDesplegado) == LOW) {
      Serial.println("Llegó a DESPLEGADO");
      detenerMotor();
      delay(2000);

      estadoActual = MOVIENDO_A_PLEGAR;
      moverPlegar();
    }
  }

  else if (estadoActual == MOVIENDO_A_PLEGAR) {

    if (digitalRead(finPlegado) == LOW) {
      Serial.println("Llegó a PLEGADO");
      detenerMotor();
      delay(2000);

      estadoActual = MOVIENDO_A_DESPLEGAR;
      moverDesplegar();
    }
  }
}