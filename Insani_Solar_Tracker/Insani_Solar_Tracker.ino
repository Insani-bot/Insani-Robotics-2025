#include <ESP32Servo.h>

// Antes de poner a funcionar este codigo por favor no conectes la trasmision del servomotor al sistema
//ponlo a funcionar sin levas o engranes asi sabes cuando es 40 grados o 105 grados
Servo A_elevacion;
int servovert = 40; // Posición inicial del servomotor
int servovertLimitHigh = 105; // Límite superior del servomotor
int servovertLimitLow = 40; // Límite inferior del servomotor

// Revisa el esquemático 
#define ldrlt 36 // LDR TL
#define ldrrt 39 // LDR TR
#define ldrld 26 // LDR BL
#define ldrrd 34 // LDR BR

int servoPin = 23; // Pin del actuador (solo control)

// Variables para el filtro de Media Móvil Exponencial (EMA)
float emaLt = 0, emaRt = 0, emaLd = 0, emaRd = 0; // Valores EMA para los LDR
float alpha = 0.07; // COEFICIENTE de suavizado para EMA
int lastMovementTime = 0; // Último momento en el que se movió el servomotor
const int movementDelay = 50; // Esperar 1s antes de mover de

void setup() {
  Serial.begin(115200); // Inicializar la comunicación serial a 115200 baudios
  // Asignación de temporizadores PWM para el ESP32
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  // Configuración del servomotor
  A_elevacion.setPeriodHertz(60); // Frecuencia del servomotor
  A_elevacion.attach(servoPin, 400, 2300); // Configuración del pin del servomotor y rango PWM
  A_elevacion.write(servovert); // Establecer la posición inicial del servomotor

  // Declaración de entradas
  pinMode(ldrlt, INPUT);
  pinMode(ldrrt, INPUT);
  pinMode(ldrld, INPUT);
  pinMode(ldrrd, INPUT);
  delay(1000); // Pequeña pausa para estabilizar el sistema
}

void loop() {
  // Mover el seguidor solar en función de las lecturas de los sensores
  // Solo mover si ha pasado el tiempo mínimo
  if (millis() - lastMovementTime > movementDelay) {
    // Leer valores analógicos de los LDR
    int lt = analogRead(ldrlt); // LDR superior izquierdo
    int rt = analogRead(ldrrt); // LDR superior derecho
    int ld = analogRead(ldrld); // LDR inferior izquierdo
    int rd = analogRead(ldrrd); // LDR inferior derecho

    // Aplicar el filtro EMA a las lecturas de los LDR
    emaLt = calculateEMA(lt, emaLt, alpha);
    emaRt = calculateEMA(rt, emaRt, alpha);
    emaLd = calculateEMA(ld, emaLd, alpha);
    emaRd = calculateEMA(rd, emaRd, alpha);

    // Promedio de los valores suavizados de los sensores superiores e inferiores
    int avt = (emaLt + emaRt) / 2; // Promedio de los sensores superiores
    int avd = (emaLd + emaRd) / 2; // Promedio de los sensores inferiores

    //Verificacion
    //  Serial.print(emaLt);
    //  Serial.print("    ");
    //  Serial.print(emaRt);
    //  Serial.print("   ");
    //  Serial.print(emaLd);
    //  Serial.print("  ema ");
    //  Serial.println(emaRd);
    //    Serial.print("Arriba ");
    //    Serial.print(avt);
    //    Serial.print(" Abajo ");
    //    Serial.println(avd);

    moveSolarTracker(avd, avt);
    lastMovementTime = millis();

    ceros(emaLt, emaRt, emaLd, emaRd);// funcion cuando la luz del dia finaliza
  }

}

// Función para mover el seguidor solar
void moveSolarTracker(int avt, int avd) {
  const int tol = 20; // Umbral de tolerancia para el movimiento
  int dvert = avt - avd; // Diferencia entre los valores superiores e inferiores

  // Comparaciones con el umbral
  if (abs(dvert) > tol) {
    if (dvert > 0 && servovert < servovertLimitHigh) {
      servovert++; // Incrementar la posición del servomotor
    } else if (dvert < 0 && servovert > servovertLimitLow) {
      servovert--; // Decrementar la posición del servomotor
    }
    A_elevacion.write(servovert); // Actualizar la posición del servomotor
    Serial.print(" Elevacion: ");// angulo de compensación
    Serial.println(servovert);
  }
}

// Calcular Media Móvil Exponencial (EMA) y estabilizacion de señal
float calculateEMA(float currentValue, float previousEMA, float alpha) {
  return (alpha * currentValue) + ((1 - alpha) * previousEMA);
}

// funcion cuando la luz del dia finaliza
void ceros(int lt, int rt, int ld, int rd) {
  int threshold = 1000; // Umbral para las lecturas de los LDR
  int targetPosition = 40; // Posición objetivo del servomotor
  int stepDelay = 50; // Retardo entre cada paso para un movimiento suave

  // Verificar si todas las lecturas de los LDR están por debajo del umbral
  if (lt < threshold && rt < threshold && ld < threshold && rd < threshold) {
    // Mover el servomotor lentamente hacia la posición objetivo
    while (servovert > targetPosition) {
      servovert--;
      A_elevacion.write(servovert);
      delay(stepDelay); // Retardo para suavizar el movimiento
    }

    while (servovert < targetPosition) {
      servovert++;
      A_elevacion.write(servovert);
      delay(stepDelay); // Retardo para suavizar el movimiento
    }
  }
}
