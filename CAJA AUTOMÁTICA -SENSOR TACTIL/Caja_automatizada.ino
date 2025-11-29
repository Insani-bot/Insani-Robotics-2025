// ======================== CONFIG SERVOS ==========================
const int servo1Pin = 18;
const int servo2Pin = 19;
const int servo3Pin = 21;

const int freq = 50;       
const int pwm1 = 0;
const int pwm2 = 1;
const int pwm3 = 2;
const int resolution = 16;

// ======================== TOUCH SENSOR ===========================
const int touchPin = 34;

volatile bool touchFlag = false;

// ======================== CONTROL ================================
enum State { IDLE, OPENING, CLOSING };
State currentState = IDLE;

int pos1 = 0, pos2 = 0, pos3 = 0;
int target1 = 0, target2 = 0, target3 = 0;

// Orden dinámico real
int order[3];  

unsigned long lastStepTime = 0;
const int stepDelay = 15;  // suavidad


// ====================== UTILIDADES ===============================
uint16_t angleToDuty(int angle) {
    angle = constrain(angle, 0, 90);
    int pulse = map(angle, 0, 180, 500, 2400);
    return (pulse * 65535) / 20000;
}

void writeServo(int channel, int angle) {
    ledcWrite(channel, angleToDuty(angle));
}


// ======================== INTERRUPCIÓN ===========================
void IRAM_ATTR touchInterrupt() {
    touchFlag = true;
}


// ======================== SETUP ==================================
void setup() {
    Serial.begin(115200);

    ledcSetup(pwm1, freq, resolution);
    ledcSetup(pwm2, freq, resolution);
    ledcSetup(pwm3, freq, resolution);

    ledcAttachPin(servo1Pin, pwm1);
    ledcAttachPin(servo2Pin, pwm2);
    ledcAttachPin(servo3Pin, pwm3);

    writeServo(pwm1, pos1);
    writeServo(pwm2, pos2);
    writeServo(pwm3, pos3);

    pinMode(touchPin, INPUT);
    attachInterrupt(digitalPinToInterrupt(touchPin), touchInterrupt, RISING);

    Serial.println("Listo con orden dinámico");
}


// ======================== LOOP ===================================
void loop() {

    // ======== EVENTO POR TOQUE ========
    if (touchFlag) {
        touchFlag = false;

        if (currentState == IDLE) {

            if (pos1 < 45) {              // → Abrir
                currentState = OPENING;
                target1 = 90; target2 = 90; target3 = 90;

                order[0] = 1; 
                order[1] = 2; 
                order[2] = 3;

                Serial.println("→ Apertura (1 → 2 → 3)");
            }
            else {                         // → Cerrar
                currentState = CLOSING;
                target1 = 0; target2 = 0; target3 = 0;

                order[0] = 3; 
                order[1] = 2; 
                order[2] = 1;

                Serial.println("→ Cierre (3 → 2 → 1)");
            }
        }
        else {
            // Cambiar dirección inmediatamente
            if (currentState == OPENING) {
                currentState = CLOSING;

                target1 = 0; target2 = 0; target3 = 0;

                order[0] = 3; order[1] = 2; order[2] = 1;

                Serial.println("→ REVERSA ahora CERRANDO (3→2→1)");
            }
            else {
                currentState = OPENING;

                target1 = 90; target2 = 90; target3 = 90;

                order[0] = 1; order[1] = 2; order[2] = 3;

                Serial.println("→ REVERSA ahora ABRIENDO (1→2→3)");
            }
        }
    }

    // ======== MOVIMIENTO NO BLOQUEANTE ========
    if (millis() - lastStepTime >= stepDelay) {
        lastStepTime = millis();

        if (currentState == IDLE) return;

        bool stageDone = true;

        // Ejecutar orden dinámico
        for (int i = 0; i < 3; i++) {

            int servo = order[i];  
            int *pos;
            int *target;
            int channel;

            if (servo == 1) {
                pos = &pos1; target = &target1; channel = pwm1;
            } else if (servo == 2) {
                pos = &pos2; target = &target2; channel = pwm2;
            } else {
                pos = &pos3; target = &target3; channel = pwm3;
            }

            if (*pos < *target) {
                (*pos)++;
                stageDone = false;
                writeServo(channel, *pos);
                break;
            }
            else if (*pos > *target) {
                (*pos)--;
                stageDone = false;
                writeServo(channel, *pos);
                break;
            }
        }

        // Si terminó los 3 → IDLE
        if (stageDone) {
            currentState = IDLE;
            Serial.println("→ Secuencia completa ✓");
        }
    }
}
