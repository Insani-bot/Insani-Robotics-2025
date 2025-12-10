#include <Arduino.h>
#include <WiFi.h>
#include "AsyncTCP.h"
#include "ESPAsyncWebServer.h"

// ================== CONFIG WiFi AP ==================
const char* ssid = "CARNAGE"; //conéctate a esta red
const char* password = "12345678";      //contraseña

// Pines motor (ejemplo con L293D)
const int motorPWM = 4;

const int MA1 = 13;
const int MA2 = 12;

const int MB1 = 14;
const int MB2 = 27;

// Canal PWM
const int motorChannel = 0;
const int freq = 5000;
const int resolution = 8;

int velocidad = 180; // valor inicial del slider

AsyncWebServer server(80);

// ================== HTML ==================
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="es">
<head>
  <meta charset="UTF-8">
  <title>Insani Virtual Joystick</title>
  <style>
    body { 
      font-family: Arial, sans-serif; 
      text-align: center; 
      margin: 0; 
      padding: 30px;
      background: linear-gradient(to bottom right, indigo, #1c5ea4); 
      color: white;
      height: 100vh;
    }   
    #areaJoystick {
      width: 600px;
      height: 600px;
      border: 30px solid rgba(48, 147, 245, 0.117);
      margin: 30px auto;
      position: relative;
      background-color: #f0f0f0;
      border-radius: 45%;
      touch-action: none;
      z-index: 0;
    }

    #areaJoystick::before,
    #areaJoystick::after {
      content: "";
      position: absolute;
      top: 50%;
      left: 50%;
      width: 103%;
      height: 3px;
      background: rgba(106, 102, 102, 0.25);
      transform-origin: center;
      z-index: 0;
      border-radius: 50px;
    }
    #areaJoystick::before { transform: translate(-50%, -50%) rotate(45deg); }
    #areaJoystick::after { transform: translate(-50%, -50%) rotate(-45deg); }
    
    #centrito {
      width: 270px;
      height: 270px;
      background-color: rgb(97, 107, 103);
      border-radius: 50%;
      position: absolute;
      top: 50%;
      left: 50%;
      transform: translate(-50%, -50%);
      pointer-events: none;
      z-index: 1;
    }
    #eje {
      width: 120px;
      height: 120px;
      border: 15px solid rgba(12, 11, 11, 0.652);
      background-color: rgba(227, 233, 230, 0.466);
      border-radius: 50%;
      position: absolute;
      top: 50%;
      left: 50%;
      transform: translate(-50%, -50%);
      pointer-events: none;
      z-index: 2;
    }
    #punto {
      width: 270px;
      height: 270px;
      border: 30px solid rgba(168, 4, 4, 0.157);
      background-color: rgb(253, 49, 3);
      border-radius: 50%;
      position: absolute;
      top: 22.5%;
      left: 22.5%;
      pointer-events: none;
      z-index: 3;
    }
    h2 {
      margin: 60px;
      font-size: 50px;
      color: white;
    }

    .flecha {
      position: absolute;
      width: 75px;
      height: 38px;
      background: rgba(0, 0, 0, 0.25);
      border-radius: 12px;
      z-index: 0;
    }
    .flecha.arriba { top: 5%; left: 50%; transform: translateX(-50%); clip-path: polygon(50% 0%, 100% 100%, 0% 100%); }
    .flecha.abajo { bottom: 5%; left: 50%; transform: translateX(-50%) rotate(180deg); clip-path: polygon(50% 0%, 100% 100%, 0% 100%); }
    .flecha.izquierda { left: 5%; top: 50%; transform: translateY(-50%) rotate(-90deg); clip-path: polygon(50% 0%, 100% 100%, 0% 100%); }
    .flecha.derecha { right: 5%; top: 50%; transform: translateY(-50%) rotate(90deg); clip-path: polygon(50% 0%, 100% 100%, 0% 100%); }
    
    #posiciones { font-size: 40px; margin-top: 20px; color: rgb(184, 195, 184); display:inline-block; margin-right:40px; }
    #control { font-size: 40px; margin-top: 10px;color: rgb(14, 190, 239); display:inline-block; margin-left:40px; }
    
    #velocidad {
      -webkit-appearance: none;
      width: 70%;
      height: 50px;
      border-radius: 15px;
      background: linear-gradient(to right, rgb(196, 226, 244), #6598c1, #0d90e1);
      outline: none;
      margin-top: 5px;
    }
    #velocidad::-webkit-slider-thumb {
      -webkit-appearance: none;
      appearance: none;
      width: 40px;
      height: 70px;
      border-radius: 15px;
      background: #444;
      cursor: pointer;
      border: none;
      box-shadow: 0 0 5px rgba(0,0,0,0.5);
    }
    #velocidad::-moz-range-thumb {
      width: 40px;
      height: 70px;
      border-radius: 15px;
      background: #444;
      cursor: pointer;
      border: none;
      box-shadow: 0 0 5px rgba(0,0,0,0.5);
    }

    #botonesLR {
      margin-top: 30px;
      display: flex;
      justify-content: center; /* centra el grupo */
      gap: 250px;               /* espacio entre L y R */
    }
    .btn {
      font-size: 36px;
      font-weight: bold;
      padding: 35px 120px;
      margin: 10px;
      border: none;
      border-radius: 30px;
      background: #333;
      color: white;
      cursor: pointer;
      transition: transform 0.2s, background 0.2s;
    
      user-select: none;          /* 🔹 evita seleccionar texto */
      -webkit-user-select: none;
      -moz-user-select: none;
      -ms-user-select: none;
      -webkit-tap-highlight-color: transparent; /* 🔹 elimina el flash azul en móviles */
    }
    
    .btn:active {
      transform: scale(0.95);
      background: #666;
    }

  </style>
</head>
<body>
  <h2>INSANI SUPER V-JOYSTICK</h2>
    <!-- Botones L y R -->
   <div id="botonesLR">
    <button id="btnL" class="btn">L</button>
    <button id="btnR" class="btn">R</button>
  </div>
  
  <div id="areaJoystick">
    <div id="punto"></div>
    <div id="centrito"></div>
    <div id="eje"></div>
    <div class="flecha arriba"></div>
    <div class="flecha abajo"></div>
    <div class="flecha izquierda"></div>
    <div class="flecha derecha"></div>
  </div>
  <p id="posiciones">X: 0, Y: 0</p>
  <p id="control">Centro</p>

  <!-- Slider -->

  <p style="font-size:40px; font-weight:bold; color:white; margin-top:120px;">Velocidad</p>
  <input type="range" id="velocidad" min="180" max="255" value="180">
  <p id="valorVelocidad" style="font-size:36px; color:white; text-align:left; margin-left:150px;">180</p>

  <script>
  const area = document.getElementById("areaJoystick");
  const punto = document.getElementById("punto");
  const posiciones = document.getElementById("posiciones");
  const control = document.getElementById("control");

  const slider = document.getElementById("velocidad");
  const valorVelocidad = document.getElementById("valorVelocidad");

  const btnL = document.getElementById("btnL");
  const btnR = document.getElementById("btnR");

  const areaSize = 600;
  const puntoSize = 330;
  const center = (areaSize - puntoSize) / 2;

  function enviarDatos(x, y) {
    fetch(`/joystick?x=${x}&y=${y}`);
  }

  function moverPunto(e) {
    e.preventDefault();
    let rect = area.getBoundingClientRect();
    let x, y;

    if(e.touches) {
      x = e.touches[0].clientX - rect.left - puntoSize/2;
      y = e.touches[0].clientY - rect.top - puntoSize/2;
    } else {
      x = e.clientX - rect.left - puntoSize/2;
      y = e.clientY - rect.top - puntoSize/2;
    }

    x = Math.max(0, Math.min(areaSize - puntoSize, x));
    y = Math.max(0, Math.min(areaSize - puntoSize, y));

    punto.style.left = x + "px";
    punto.style.top = y + "px";

    let xRel = Math.round(((x - center) / center) * 100);
    let yRel = Math.round(((center - y) / center) * 100);

    posiciones.textContent = `X: ${xRel}, Y: ${yRel}`;

    const umbral = 30;
    let dir = "Centro";
    if (Math.abs(xRel) > umbral || Math.abs(yRel) > umbral) {
      if (Math.abs(xRel) > Math.abs(yRel)) {
        dir = (xRel > 0) ? "Derecha" : "Izquierda";
      } else {
        dir = (yRel > 0) ? "Arriba" : "Abajo";
      }
    }
    control.textContent = dir;
    enviarDatos(xRel, yRel);
  }

  // Ratón
  area.addEventListener("mousedown", e => {
    moverPunto(e);
    area.addEventListener("mousemove", moverPunto);
  });
  document.addEventListener("mouseup", () => {
    area.removeEventListener("mousemove", moverPunto);
    punto.style.left = center + "px";
    punto.style.top = center + "px";
    posiciones.textContent = "X: 0, Y: 0";
    control.textContent = "Centro";
    enviarDatos(0, 0);
  });

  // Táctil
  area.addEventListener("touchstart", moverPunto);
  area.addEventListener("touchmove", moverPunto);
  area.addEventListener("touchend", () => {
    punto.style.left = center + "px";
    punto.style.top = center + "px";
    posiciones.textContent = "X: 0, Y: 0";
    control.textContent = "Centro";
    enviarDatos(0, 0);
  });

  // Slider
  slider.addEventListener("change", () => {
    let v = slider.value;
    valorVelocidad.textContent = v;
    fetch(`/velocidad?valor=${v}`);
  });

  // Botones L y R (mantener pulsado)
  function enviarBoton(nombre, estado) {
    fetch(`/boton?btn=${nombre}&estado=${estado}`);
  }

  btnL.addEventListener("mousedown", () => enviarBoton("L", 1));
  btnL.addEventListener("mouseup", () => enviarBoton("L", 0));
  btnL.addEventListener("touchstart", () => enviarBoton("L", 1));
  btnL.addEventListener("touchend", () => enviarBoton("L", 0));

  btnR.addEventListener("mousedown", () => enviarBoton("R", 1));
  btnR.addEventListener("mouseup", () => enviarBoton("R", 0));
  btnR.addEventListener("touchstart", () => enviarBoton("R", 1));
  btnR.addEventListener("touchend", () => enviarBoton("R", 0));
  </script>

</body>
</html>
)rawliteral";

// ================== SETUP ==================
void setup() {
  Serial.begin(115200);

  pinMode(MA1, OUTPUT);
  pinMode(MA2, OUTPUT);
  
  pinMode(MB1, OUTPUT);
  pinMode(MB2, OUTPUT);

  //pinMode(2, OUTPUT);

  ledcSetup(motorChannel, freq, resolution);
  ledcAttachPin(motorPWM, motorChannel);

  ledcWrite(motorChannel, velocidad);

  WiFi.softAP(ssid, password);
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/html", index_html);
  });

  // Joystick
  server.on("/joystick", HTTP_GET, [](AsyncWebServerRequest * request) {
  if (request->hasParam("x") && request->hasParam("y")) {
    int x = request->getParam("x")->value().toInt();
    int y = request->getParam("y")->value().toInt();
    Serial.printf("JOYSTICK -> X:%d Y:%d\n", x, y);

    // --- Control de motores según dirección ---
    const int umbral = 50;

    if (y > umbral && abs(x) < umbral) {
      // ↑ ARRIBA → MA + MB avanzan
      Serial.println("JOYSTICK: AVANZA (MA+MB)");
      digitalWrite(MA1, 1);
      digitalWrite(MA2, 0);

      digitalWrite(MB1, 0);
      digitalWrite(MB2, 1);

      ledcWrite(motorChannel, velocidad);
    }
    else if (y < -umbral && abs(x) < umbral) {
      // ← ABAJO 
      Serial.println("JOYSTICK: GIRA IZQUIERDA (MB+MC)");
      digitalWrite(MA1, 0);
      digitalWrite(MA2, 1);

      digitalWrite(MB1, 1);
      digitalWrite(MB2, 0);

      ledcWrite(motorChannel, velocidad);
    }  
    else if (x < -umbral && abs(y) < umbral) {
      // ← IZQUIERDA → MB + MC
      Serial.println("JOYSTICK: GIRA IZQUIERDA (MB+MC)");
      digitalWrite(MA1, 0);
      digitalWrite(MA2, 1);

      digitalWrite(MB1, 0);
      digitalWrite(MB2, 1);

      ledcWrite(motorChannel, velocidad);
    } 
    else if (x > umbral && abs(y) < umbral) {
      // → DERECHA → MA + MC
      Serial.println("JOYSTICK: GIRA DERECHA (MA+MC)");
      digitalWrite(MA1, 1);
      digitalWrite(MA2, 0);

      digitalWrite(MB1, 1);
      digitalWrite(MB2, 0);

      ledcWrite(motorChannel, velocidad);
    } 
    else {
      // CENTRO→ PARAR TODO
      Serial.println("JOYSTICK: CENTRO (STOP)");
      ledcWrite(motorChannel, 0);

      digitalWrite(MA1, 0);
      digitalWrite(MA2, 0);

      digitalWrite(MB1, 0);
      digitalWrite(MB2, 0);
    }
  }

  request->send(200, "text/plain", "OK");
});

  // Velocidad
  server.on("/velocidad", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (request->hasParam("valor")) {
      velocidad = request->getParam("valor")->value().toInt();
      Serial.printf("VELOCIDAD -> %d\n", velocidad);
      ledcWrite(motorChannel, velocidad);
    }
    request->send(200, "text/plain", "OK");
  });

  // Botones L y R
  server.on("/boton", HTTP_GET, [](AsyncWebServerRequest * request) {
  if (request->hasParam("btn") && request->hasParam("estado")) {
    String btn = request->getParam("btn")->value();
    int estado = request->getParam("estado")->value().toInt();
    Serial.printf("BOTON_%s -> %d\n", btn.c_str(), estado);

  }
  request->send(200, "text/plain", "OK");
  });


  server.begin();
}

void loop() {
  // AsyncWebServer maneja todo
}
