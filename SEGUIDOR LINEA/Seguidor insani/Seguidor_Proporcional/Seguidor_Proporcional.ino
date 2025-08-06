#define m1 5  //Right Motor MA1
#define m2 6  //Right Motor MA2

#define m3 2  //Left Motor MB1
#define m4 4  //Left Motor MB2

#define e1 9  //Right Motor Enable Pin EA
#define e2 3 //Left Motor Enable Pin EB

int velocidad = 80;

int s[6];
int threshold = 480; //set threshold as like i've shown in video
int position[6] = { 1, 2, 3, 4, 5 };

//push button and led
int button_pin = 12;
int led = 13;
bool button_pin_state = false;

void setup() {

  pinMode(button_pin, INPUT_PULLUP);
  pinMode(led, OUTPUT);

  pinMode(m1, OUTPUT);
  pinMode(m2, OUTPUT);
  pinMode(m3, OUTPUT);
  pinMode(m4, OUTPUT);

  pinMode(e1, OUTPUT);
  pinMode(e2, OUTPUT);

  Serial.begin(9600);

}

void loop() {
  button_pin_state = digitalRead(button_pin);
  display_value();

  while (button_pin_state != 1) {
    //blink led
    //    for (int i = 0; i < 3; ++i) {
    //      digitalWrite(led, HIGH);
    //      delay(100);
    //      digitalWrite(led, LOW);
    //      delay(100);
    //    }
    Seguidor();  //line follow using PID_Value

  }
}


void Seguidor() {
  for (byte i = 0; i < 5; i++) {  //sensor data read from A0, A1, A2, A6, A7
    if (i > 2) {
      s[i] = analogRead(i + 3);
    } else {
      s[i] = analogRead(i);
    }
    if (s[i] > threshold)
      s[i] = 1;  //analog value to digital conversion
    else
      s[i] = 0;
  }
  //if only middle sensor detects black line
  if ((s[0] == 0) && (s[1] == 0) && (s[2] == 1) && (s[3] == 0) && (s[4] == 0)) {
    //going forward with full speed
    analogWrite(e1, velocidad + 150); //you can adjust the speed of the motors from 0-255
    analogWrite(e2, velocidad + 150); //you can adjust the speed of the motors from 0-255

    digitalWrite(m1, HIGH);
    digitalWrite(m2, LOW);

    digitalWrite(m3, HIGH);
    digitalWrite(m4, LOW);
    Serial.println("Recto");
  }

  //if only left sensor detects black line
  if ((s[0] == 0) && (s[1] == 0) && (s[2] == 0) && (s[3] == 1) && (s[4] == 0)) {
    //going right with full speed
    analogWrite(e1, velocidad); //you can adjust the speed of the motors from 0-255
    analogWrite(e2, velocidad); //you can adjust the speed of the motors from 0-255
    digitalWrite(m1, HIGH);
    digitalWrite(m2, LOW);
    digitalWrite(m3, LOW);
    digitalWrite(m4, LOW);
    Serial.println("Poquito a la derecha");
  }

  //if only left most sensor detects black line
  if ((s[0] == 0) && (s[1] == 0) && (s[2] == 0) && (s[3] == 0) && (s[4] == 1)) {
    //going right with full speed
    analogWrite(e1, velocidad); //you can adjust the speed of the motors from 0-255
    analogWrite(e2, velocidad); //you can adjust the speed of the motors from 0-255
    digitalWrite(m1, HIGH);
    digitalWrite(m2, LOW);

    digitalWrite(m3, LOW);
    digitalWrite(m4, HIGH);
    Serial.println("Mucho a la derecha");
  }

  //if only right sensor detects black line
  if ((s[0] == 0) && (s[1] == 1) && (s[2] == 0) && (s[3] == 0) && (s[4] == 0)) {
    //going left with full speed
    analogWrite(e1, velocidad); //you can adjust the speed of the motors from 0-255
    analogWrite(e2, velocidad); //you can adjust the speed of the motors from 0-255
    digitalWrite(m1, LOW);
    digitalWrite(m2, LOW);
    digitalWrite(m3, HIGH);
    digitalWrite(m4, LOW);
    Serial.println("Poquito a la Izquierda");
  }

  //if only right most sensor detects black line
  if ((s[0] == 1) && (s[1] == 0) && (s[2] == 0) && (s[3] == 0) && (s[4] == 0)) {
    //going left with full speed
    analogWrite(e1, velocidad); //you can adjust the speed of the motors from 0-255
    analogWrite(e2, velocidad); //you can adjust the speed of the motors from 0-255
    digitalWrite(m1, LOW);
    digitalWrite(m2, HIGH);
    digitalWrite(m3, HIGH);
    digitalWrite(m4, LOW);
    Serial.println("Mucho a la Izquierda");
  }

  //if middle and right sensor detects black line
  if ((s[0] == 0) && (s[1] == 1) && (s[2] == 1) && (s[3] == 0) && (s[4] == 0)) {
    //going left with full speed
    analogWrite(e1, velocidad); //you can adjust the speed of the motors from 0-255
    analogWrite(e2, velocidad); //you can adjust the speed of the motors from 0-255
    digitalWrite(m1, LOW);
    digitalWrite(m2, LOW);
    digitalWrite(m3, HIGH);
    digitalWrite(m4, LOW);
    Serial.println("Alinear por Izquierda");
  }

  //if middle and left sensor detects black line
  if ((s[0] == 0) && (s[1] == 0) && (s[2] == 1) && (s[3] == 1) && (s[4] == 0)) {
    //going right with full speed
    analogWrite(e1, velocidad); //you can adjust the speed of the motors from 0-255
    analogWrite(e2, velocidad); //you can adjust the speed of the motors from 0-255
    digitalWrite(m1, HIGH);
    digitalWrite(m2, LOW);
    digitalWrite(m3, LOW);
    digitalWrite(m4, LOW);
    Serial.println("Alinear por Derecha");
  }

  //if middle, left and left most sensor detects black line
  if ((s[0] == 0) && (s[1] == 0) && (s[2] == 1) && (s[3] == 1) && (s[4] == 1)) {
    //going right with full speed
    analogWrite(e1, velocidad); //you can adjust the speed of the motors from 0-255
    analogWrite(e2, velocidad); //you can adjust the speed of the motors from 0-255
    digitalWrite(m1, HIGH);
    digitalWrite(m2, LOW);
    digitalWrite(m3, LOW);
    digitalWrite(m4, LOW);
    Serial.println("Alinear mucho por Derecha");
  }

  //if middle, right and right most sensor detects black line
  if ((s[0] == 1) && (s[1] == 1) && (s[2] == 1) && (s[3] == 0) && (s[4] == 0)) {
    //going left with full speed
    analogWrite(e1, velocidad); //you can adjust the speed of the motors from 0-255
    analogWrite(e2, velocidad); //you can adjust the speed of the motors from 0-255
    digitalWrite(m1, LOW);
    digitalWrite(m2, LOW);
    digitalWrite(m3, HIGH);
    digitalWrite(m4, LOW);
    Serial.println("Alinear mucho por Izquierda");
  }

  //if all sensors are on a black line
  if ((s[0] == 1) && (s[1] == 1) && (s[2] == 1) && (s[3] == 1) && (s[4] == 1)) {
    //stop
    digitalWrite(m1, LOW);
    digitalWrite(m2, LOW);
    digitalWrite(m3, LOW);
    digitalWrite(m4, LOW);
    Serial.println("PARAR");
  }

}

void display_value() {  //display the analog value of sensor in serial monitor
  for (byte i = 0; i < 5; i++) {  //sensor data read from A0, A1, A2, A6, A7
    if (i > 2) {
      s[i] = analogRead(i + 3);
    } else {
      s[i] = analogRead(i);
    }
    if (s[i] > threshold)
      s[i] = 1;  //analog value to digital conversion
    else
      s[i] = 0;
    Serial.print(String(s[i]) + " ");
  }
  Serial.println();
  delay(50);
}
