#include <WiFi.h>
#include <Wire.h>   
#include <LCD.h>     
#include <AsyncTCP.h>
#include <AsyncWebSocket.h>
#include <DHT.h>
#include <DHT_U.h>
#include "Paginawebcode.h"
#include <LiquidCrystal_I2C.h>    
#include <ArduinoJson.h> 

int SENSOR = 27; //dht11
int LED = 2;
int VENT1 = 18; //motor
int VENT2 = 19;
int ENA = 5;
int BUZZ= 33;

const int canalPWM = 0;  
const int resolucion = 10;     //rango 600-1023
const int frecuencia = 3000;   // subir frecuencia entre 1-5
int velocidad = 2;
int duty = 0;


int TEMPERATURA;
int HUMEDAD;
int alarma = 25;

float SETPOINT = 50;
float TEMP = 0.0;
// Variable para controlar el estado de la alarma
bool ALARMA_ACTIVADA = false;
bool ALARMA_DISPARADA = false;   // ya ocurrió

unsigned long ultimoTiempo;
int horas = 0;
int minutos = 0;
int segundos = 0;

DHT dht(SENSOR, DHT11);

LiquidCrystal_I2C lcd (0x27, 2, 1, 0, 4, 5, 6, 7); // DIR, E, RW, RS, D4, D5, D6, D7

// Creación de una instancia del servidor web en el puerto 80
AsyncWebServer server(80);
AsyncWebSocket webSocket("/ws");

void setup() {
  Serial.begin(115200);
  Serial.println("\n");

  dht.begin();
   lcd.setBacklightPin(3,POSITIVE); 
    lcd.setBacklight(HIGH);   
    lcd.begin(16, 2); // 16 columnas por 2 lineas
    lcd.clear();// limpia pantalla 


//WiFi//
  WiFi.begin("iPhone de Santi", "santi1234");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("********************************************");
  Serial.print("Conectado a la red WiFi: ");
  Serial.println(WiFi.SSID());
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
  Serial.println("*********************************************");

  // Configuración de la respuesta a la ruta "/" del servidor web
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    // Se envía el contenido HTML almacenado en Paginawebcode
    request->send(200, "text/html", Paginawebcode);
  });
  ////

server.on("/apagarAlarma", HTTP_GET, [](AsyncWebServerRequest *request){
    ALARMA_ACTIVADA = false;   // apaga LED/buzzer
    ALARMA_DISPARADA = false;  

    request->send(200, "text/plain", "Alarma apagada");
    StaticJsonDocument<50> doc;
    doc["alarma"] = false;
    String msg;
    serializeJson(doc, msg);
    webSocket.textAll(msg);
});

  server.begin();
  webSocket.onEvent(onWebSocketEvent); // Asignación de la función de manejo de eventos al WebSocket

  server.addHandler(&webSocket); // Agregar el WebSocket al servidor

  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

  pinMode(VENT1, OUTPUT);
  pinMode(VENT2, OUTPUT);
  pinMode(ENA, OUTPUT);
  digitalWrite(VENT1, LOW);
  digitalWrite(VENT2, LOW); 

  ledcAttachPin(ENA, canalPWM);
  ledcSetup(canalPWM, frecuencia, resolucion);
  
  pinMode(BUZZ, OUTPUT);
   digitalWrite(BUZZ, LOW); 

  ultimoTiempo = millis();
}

void loop() {

  int duty = map(velocidad, 1, 5, 650, 1023);
  ledcWrite(canalPWM, duty);
  
  unsigned long ahora = millis();

  if (ahora - ultimoTiempo >= 1000) { // 1 segundo
    ultimoTiempo += 1000;
    segundos++;

    if (segundos >= 60) {
        segundos = 0;
        minutos++;

        if (minutos >= 60) {
            minutos = 0;
            horas++;

            if (horas >= 24) {
                horas = 0;
            }
        }
    }
  }

  Serial.print("Hora: ");
  Serial.print(horas);
  Serial.print(":");
  Serial.print(minutos);
  Serial.print(":");
  Serial.println(segundos);


  TEMPERATURA = dht.readTemperature();
  HUMEDAD = dht.readHumidity();
  lcd.setCursor(0, 0);
  lcd.print("Temp:");
  lcd.print(TEMPERATURA);

  lcd.setCursor(0, 1);
  lcd.print("Hora:");
  lcd.print(horas);
  lcd.print(":");
  if(minutos < 10) lcd.print("0");
  lcd.print(minutos);

  delay(100);

//printea en la pagina//
  StaticJsonDocument<100> jsonDoc;
  jsonDoc["temperatura"] = TEMPERATURA;
  jsonDoc["humedad"] = HUMEDAD;
  String jsonStr;
  serializeJson(jsonDoc, jsonStr);
  webSocket.textAll(jsonStr);
////

  //VENTILADOR//
  if (TEMPERATURA >= SETPOINT) {
        digitalWrite(VENT2, HIGH);
        digitalWrite(VENT1, LOW);
        duty = 350;
        ledcWrite(canalPWM, duty);
  } else {
        digitalWrite(VENT2, LOW);
        digitalWrite(VENT1, LOW);
        duty = 0;
        ledcWrite(canalPWM, duty);
  }
  ////

  //ESTUFA//
  if (TEMPERATURA <= TEMP) {
      digitalWrite(2, HIGH);
  } else {
      digitalWrite(2, LOW);
  }
  ////

  //ALARMA//
  if (horas == alarma && segundos == 0 && minutos == 0 && !ALARMA_DISPARADA) {
      ALARMA_ACTIVADA = true;   // encender alarma
      ALARMA_DISPARADA = true;  // marcar que ya sonó este minuto
  } 
  if (segundos != 0 || minutos > 0) {
      ALARMA_DISPARADA = false;
  }

  if (ALARMA_ACTIVADA) {
      digitalWrite(BUZZ, HIGH);
  } else {
      digitalWrite(BUZZ, LOW);
  }

  delay(100);
}

void onWebSocketEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
    if (type == WS_EVT_DATA) {
        AwsFrameInfo *info = (AwsFrameInfo *)arg;
        if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
            // Recibir mensaje WebSocket como texto
            data[len] = '\0'; // Asegurarse de que la cadena esté terminada
            String message = (char *)data;

            // Analizar el mensaje JSON para obtener el setpoint o el estado de la alarma
            DynamicJsonDocument jsonDoc(100);
            DeserializationError error = deserializeJson(jsonDoc, message);

            if (!error) {
                if (jsonDoc.containsKey("setpoint")) {
                    SETPOINT = jsonDoc["setpoint"];
                    Serial.print("Setpoint actualizado a: ");
                    Serial.println(SETPOINT);
                }
                if (jsonDoc.containsKey("tem")) {
                    TEMP = jsonDoc["tem"];
                    Serial.print("Setpoint 2 actualizado a: ");
                    Serial.println(TEMP);
                }
              if (jsonDoc.containsKey("hora")) {
                  horas = jsonDoc["hora"];
              }
              if (jsonDoc.containsKey("minutos")) {
                  minutos = jsonDoc["minutos"];
                  segundos = 0;   // Reiniciar segundos
              }
              ultimoTiempo = millis();  // Evitar saltos

                if (jsonDoc.containsKey("alarm")) {
                    alarma = jsonDoc["alarm"];
                    Serial.print("Alarma configurada a: ");
                    Serial.println(alarma);
                }
                if (jsonDoc.containsKey("alarma")) {
                    ALARMA_ACTIVADA = jsonDoc["alarma"];
                    Serial.print("Alarma activada: ");
                    Serial.println(ALARMA_ACTIVADA ? "Sí" : "No");
                }
            }
        }
    }
}