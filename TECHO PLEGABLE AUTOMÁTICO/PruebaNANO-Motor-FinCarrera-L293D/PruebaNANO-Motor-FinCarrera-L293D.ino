// -------- MOTOR --------
const int EN = 3;     // D3 PWM
const int IN1 = 9;    //IN1 D9
const int IN2 = 10;   //IN2 D10

// -------- FINALES --------
const int finPlegado = 5;     // D5
const int finDesplegado = 6;  // D6

// -------- CONFIG --------
const int velocidadMotor = 240;

// -------- ESTADOS --------
enum Estado {
  PLEGADO,
  DESPLEGADO,
  MOVIENDO_A_DESPLEGAR,
  MOVIENDO_A_PLEGAR
};

Estado estadoActual;


// ================= MOTOR =================

void moverDesplegar() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(EN, velocidadMotor);
}

void moverPlegar() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  analogWrite(EN, velocidadMotor);
}

void detenerMotor() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  analogWrite(EN, 0);
}


// ================= SETUP =================

void setup() {

  Serial.begin(9600); // Nano trabaja mejor a 9600

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  pinMode(finPlegado, INPUT_PULLUP);
  pinMode(finDesplegado, INPUT_PULLUP);

  Serial.println("TEST IDA Y VUELTA - NANO");

  estadoActual = MOVIENDO_A_DESPLEGAR;

  moverDesplegar();
}


// ================= LOOP =================

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