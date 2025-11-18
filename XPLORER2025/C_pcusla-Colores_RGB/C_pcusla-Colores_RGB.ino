                            // Pines del LED RGB
const int LED_R = 6;
const int LED_G = 5;
const int LED_B = 3;

void setup() {
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);

  // Apagar todo al inicio
  setColor(0, 0, 0);
}

void loop() {

  // --- SECUENCIA DE COLORES FIJOS ---
  setColor(255, 0, 0);   // Rojo
  delay(2000);

  setColor(0, 255, 0);   // Verde
  delay(2000);

  setColor(0, 0, 255);   // Azul
  delay(2000);

  // --- SECUENCIA FADE SUAVE ---
  fadeColor(1);  // Fade rojo
  fadeColor(2);  // Fade verde
  fadeColor(3);  // Fade azul
}

// =========================================================
// FUNCIONES
// =========================================================

// Setear un color RGB
void setColor(int r, int g, int b) {
  analogWrite(LED_R, r);
  analogWrite(LED_G, g);
  analogWrite(LED_B, b);
}

// Fade suave según color
void fadeColor(int colorID) {

  // Subir brillo de 0 a 255
  for (int i = 0; i <= 255; i++) {
    if (colorID == 1) setColor(i, 0, 0);      // Rojo
    if (colorID == 2) setColor(0, i, 0);      // Verde
    if (colorID == 3) setColor(0, 0, i);      // Azul
    delay(5);
  }

  // Bajar brillo de 255 a 0
  for (int i = 255; i >= 0; i--) {
    if (colorID == 1) setColor(i, 0, 0);
    if (colorID == 2) setColor(0, i, 0);
    if (colorID == 3) setColor(0, 0, i);
    delay(5);
  }
}
