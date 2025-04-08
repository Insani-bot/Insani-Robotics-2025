/// Definir los pines para los 6 sensores IR Flame
#define FLAME_1 32  // Norte
#define FLAME_2 33  // Noreste
#define FLAME_3 25  // Sureste
#define FLAME_4 26  // Sur
#define FLAME_5 27  // Suroeste
#define FLAME_6 35
// Noroeste

void setup() {
  Serial.begin(115200);
  pinMode(FLAME_1, INPUT);
  pinMode(FLAME_2, INPUT);
  pinMode(FLAME_3, INPUT);
  pinMode(FLAME_4, INPUT);
  pinMode(FLAME_5, INPUT);
  pinMode(FLAME_6, INPUT);
}

void loop() {
  // Leer sensores de fuego (1 = fuego detectado, 0 = no detectado)
  int n  = analogRead(FLAME_1);
  int ne = analogRead(FLAME_2);
  int se = analogRead(FLAME_3);
  int s  = analogRead(FLAME_4);
  int sw = analogRead(FLAME_5);
  int nw = analogRead(FLAME_6);

//  Serial.print(n);
//  Serial.print(" ");
//  Serial.print(ne);
//  Serial.print(" ");
//  Serial.print(se);
//  Serial.print(" ");
//  Serial.print(s);
//  Serial.print(" ");
//  Serial.print(sw);
//  Serial.print(" ");
//  Serial.println(nw);
  //
  // Mensajes según dirección detectada
  if (n || ne || se || s || sw || nw) {
    Serial.print("🔥 Alerta: Fuego detectado en ");
    if (n < 3000)  Serial.print("Norte ");
    if (ne < 3000) Serial.print("Noreste ");
    if (se < 3000) Serial.print("Sureste ");
    if (s < 3000)  Serial.print("Sur ");
    if (sw < 3000) Serial.print("Suroeste ");
    if (nw < 3000) Serial.print("Noroeste ");
    // Si todos los sensores detectan fuego, estamos rodeados
//    if (n && ne && se && s && sw && nw) {
//      Serial.print("¡Estamos rodeados!");
//    }
    Serial.println();
  }
  delay(300);
}
