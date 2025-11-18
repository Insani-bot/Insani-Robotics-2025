// Pines del RGB
const int LED_R = 6;
const int LED_G = 5;
const int LED_B = 3;

// Función para setear color RGB
void setColor(int r, int g, int b) {
  analogWrite(LED_R, r);
  analogWrite(LED_G, g);
  analogWrite(LED_B, b);
}

void setup() {
  Serial.begin(9600);
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);

  Serial.println("Envia R, G, B, Y, C, M, W, O, P");
}

void loop() {
  if (Serial.available()) {

    char c = Serial.read();
    c = toupper(c);  // Asegura que funcione con minúsculas

    switch (c) {

      case 'R':
        Serial.println("Color: ROJO");
        setColor(255, 0, 0);
        break;

      case 'G':
        Serial.println("Color: VERDE");
        setColor(0, 255, 0);
        break;

      case 'B':
        Serial.println("Color: AZUL");
        setColor(0, 0, 255);
        break;

      // ===== Mezclas de 2 =====
      case 'Y':  
        Serial.println("Color: AMARILLO (Rojo + Verde)");
        setColor(150, 255, 0);
        break;

      case 'C':  
        Serial.println("Color: CIAN (Verde + Azul)");
        setColor(0, 255, 255);
        break;

      case 'M':  
        Serial.println("Color: MAGENTA (Rojo + Azul)");
        setColor(100, 0, 255);
        break;

      // ===== Mezclas de 3 =====
      case 'W':
        Serial.println("Color: BLANCO (R+G+B)");
        setColor(120, 255, 255);
        break;

      case 'O':
        Serial.println("Color: NARANJA (Rojo + Verde bajo)");
        setColor(160, 170, 0);
        break;

      case 'P':
        Serial.println("Color: ROSA (Rojo alto + Azul medio)");
        setColor(150, 50, 200);
        break;

      default:
        //Serial.println("Comando no reconocido");
        break;
    }
  }
}
