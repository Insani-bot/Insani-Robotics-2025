// Pines ultrasónico
const int TRIG = 13;
const int ECHO = 10;

// Pines LED RGB
const int LED_R = 6;
const int LED_G = 5;
const int LED_B = 3;

long duracion;
int distancia;

void setup() {
  Serial.begin(9600);

  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);

  setColor(0, 0, 0); // todo apagado al inicio
}

void loop() {

  // Medir distancia
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  duracion = pulseIn(ECHO, HIGH);
  distancia = duracion * 0.034 / 2;

  Serial.print("Distancia: ");
  Serial.print(distancia);
  Serial.println(" cm");

  // --- Condiciones RGB ---
  if (distancia >= 30) {
    setColor(0, 0, 255);       // Azul
  }
  else if (distancia >= 20) {
    setColor(0, 255, 0);       // Verde
  }
  else if (distancia >= 10) {
    setColor(255, 150, 0);     // Amarillo (rojo + verde)
  }
  else {
    setColor(255, 0, 0);       // Rojo
  }

  delay(150);
}

// ------------------------------
// Función para controlar el RGB
// ------------------------------
void setColor(int r, int g, int b) {
  analogWrite(LED_R, r);
  analogWrite(LED_G, g);
  analogWrite(LED_B, b);
}
