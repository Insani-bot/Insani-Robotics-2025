//EJEMPLO: Lectura analogica y Filtro

#define PinSensor 34 

float EMA = 0; // // Variables para filtro (EMA)
float alpha = 0.07; // COEFICIENTE de suavizado para EMA
int lastMovementTime = 0; //
const int movementDelay = 100; 

void setup() {
  Serial.begin(115200);
  // Entrada analógica
  pinMode(PinSensor, INPUT);
}

void loop() {
  if (millis() - lastMovementTime > movementDelay) {
    
    // Letura Analógica
    int HumedadSuelo = ((4096  - analogRead(PinSensor)) * 100) / 4095;

    EMA = alpha * HumedadSuelo + (1 - alpha) * EMA;
 
    //Verificacion
    Serial.print(HumedadSuelo); //Medida directa
    Serial.print("    ");
    Serial.println(EMA);        //Medida con filtro
    
    lastMovementTime = millis();
  }
}
