// Pines del sensor ultrasónico
const int TRIG = 13;
const int ECHO = 10;

long duracion;
int distancia;

void setup() {
  Serial.begin(9600);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
}

void loop() {

  // Asegura pulso limpio
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);

  // Pulso de activación de 10us
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  // Captura el tiempo del rebote
  duracion = pulseIn(ECHO, HIGH);

  // Convierte a distancia en cm
  distancia = duracion * 0.034 / 2;

  // Muestra en monitor serial
  Serial.print("Distancia: ");
  Serial.print(distancia);
  Serial.println(" cm");

  delay(200);
}
