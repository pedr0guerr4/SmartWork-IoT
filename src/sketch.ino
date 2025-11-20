#include <WiFi.h>
#include <PubSubClient.h>
#include "DHTesp.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const char* ssid = "Wokwi-GUEST";
const char* password = "";

const char* mqttServer   = "broker.hivemq.com";
const int   mqttPort     = 1883;
const char* mqttClientId = "SmartWork-ESP32-01";
const char* topicTelemetry = "smartwork/grupoX/telemetry";
const char* topicCommands  = "smartwork/grupoX/commands";

WiFiClient espClient;
PubSubClient client(espClient);

const int DHT_PIN = 15;
DHTesp dht;

const int LDR_PIN = 34;

const int LED_R_PIN = 18;  
const int LED_G_PIN = 19;  
const int LED_B_PIN = 23; 

const int BUZZER_PIN = 5;

const int BTN_START_FOCUS_PIN = 13; 
const int BTN_BREAK_DONE_PIN  = 12; 

LiquidCrystal_I2C lcd(0x27, 16, 2);

unsigned long lastTelemetryMillis = 0;
unsigned long telemetryInterval   = 15000;

unsigned long lastLcdUpdate = 0;
unsigned long lcdInterval   = 500;

unsigned long focusStartMillis = 0;
bool focusStarted = false;

enum StatusEnum { STATUS_OK, STATUS_ATENCAO, STATUS_CRITICO };
StatusEnum currentStatus = STATUS_OK;


void connectWiFi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
  }
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String msg;
  for (unsigned int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }

  if (msg.indexOf("forceBreak") >= 0) {
    focusStartMillis = millis();
    tone(BUZZER_PIN, 2000, 300);
  } else if (msg.indexOf("resetTimer") >= 0) {
    focusStartMillis = millis();
  }
}

void connectMQTT() {
  client.setServer(mqttServer, mqttPort);
  client.setCallback(mqttCallback);

  while (!client.connected()) {
    client.connect(mqttClientId);
    delay(300);
  }

  client.subscribe(topicCommands);
}

float calcularIndiceConforto(float temp, float hum, int luz) {
  float score = 0.0;

  if (temp >= 21 && temp <= 24) score += 0.4;
  else if (temp >= 18 && temp <= 27) score += 0.2;

  if (hum >= 40 && hum <= 60) score += 0.3;
  else if (hum >= 30 && hum <= 70) score += 0.15;

  if (luz > 1500) score += 0.3;
  else if (luz > 800) score += 0.15;

  if (score > 1.0) score = 1.0;
  return score;
}

void atualizarLedBuzzer(StatusEnum st, int luz) {

  if (luz < 400) {  
    digitalWrite(LED_B_PIN, HIGH);
  } else {
    digitalWrite(LED_B_PIN, LOW);
  }

  switch (st) {
    case STATUS_OK:
      digitalWrite(LED_R_PIN, LOW);
      digitalWrite(LED_G_PIN, HIGH);
      noTone(BUZZER_PIN);
      break;

    case STATUS_ATENCAO:
      digitalWrite(LED_R_PIN, HIGH);
      digitalWrite(LED_G_PIN, HIGH);
      noTone(BUZZER_PIN);
      break;

    case STATUS_CRITICO:
      digitalWrite(LED_R_PIN, HIGH);
      digitalWrite(LED_G_PIN, LOW);
      tone(BUZZER_PIN, 2000, 200);
      break;
  }
}

void atualizarLCD(float t, float h, int focoSeg, StatusEnum st) {
  if (millis() - lastLcdUpdate < lcdInterval) return;
  lastLcdUpdate = millis();

  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(t, 1);
  lcd.print("C H:");
  lcd.print(h, 0);
  lcd.print("%   ");

  lcd.setCursor(0, 1);
  lcd.print("F:");
  lcd.print(focoSeg);
  lcd.print("s ");

  if (st == STATUS_OK)       lcd.print("OK   ");
  else if (st == STATUS_ATENCAO) lcd.print("ATEN ");
  else                        lcd.print("CRIT ");
}

void setup() {
  Serial.begin(115200);

  pinMode(LDR_PIN, INPUT);
  pinMode(LED_R_PIN, OUTPUT);
  pinMode(LED_G_PIN, OUTPUT);
  pinMode(LED_B_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  pinMode(BTN_START_FOCUS_PIN, INPUT_PULLUP);
  pinMode(BTN_BREAK_DONE_PIN, INPUT_PULLUP);

  dht.setup(DHT_PIN, DHTesp::DHT22);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("SmartWork");
  lcd.setCursor(0, 1);
  lcd.print("Conectando...");

  connectWiFi();
  connectMQTT();

  focusStartMillis = millis();
  focusStarted = true;
}

void loop() {
  if (!client.connected()) {
    connectMQTT();
  }
  client.loop();

  if (digitalRead(BTN_START_FOCUS_PIN) == LOW) {
    focusStartMillis = millis();
    focusStarted = true;
    delay(200);
  }

  if (digitalRead(BTN_BREAK_DONE_PIN) == LOW) {
    focusStartMillis = millis();
    delay(200);
  }

  TempAndHumidity data = dht.getTempAndHumidity();
  float t = data.temperature;
  float h = data.humidity;
  int luz = analogRead(LDR_PIN);

  int focoSeg = 0;
  if (focusStarted) {
    focoSeg = (millis() - focusStartMillis) / 1000;
  }

  float confortIndex = calcularIndiceConforto(t, h, luz);

  StatusEnum st;
  if (confortIndex > 0.7 && focoSeg < 20)       st = STATUS_OK;    
  else if (confortIndex > 0.4 && focoSeg < 40)  st = STATUS_ATENCAO;  
  else                                          st = STATUS_CRITICO; 

  currentStatus = st;

  atualizarLedBuzzer(st, luz);
  atualizarLCD(t, h, focoSeg, st);

  unsigned long now = millis();
  if (now - lastTelemetryMillis > telemetryInterval) {
    lastTelemetryMillis = now;

    String statusStr = (st == STATUS_OK) ? "OK" :
                       (st == STATUS_ATENCAO) ? "ATENCAO" : "CRITICO";

    String payload = "{";
    payload += "\"temperature\":"   + String(t, 1) + ",";
    payload += "\"humidity\":"      + String(h, 1) + ",";
    payload += "\"light\":"         + String(luz) + ",";
    payload += "\"focusSeconds\":"  + String(focoSeg) + ",";
    payload += "\"comfortIndex\":"  + String(confortIndex, 2) + ",";
    payload += "\"status\":\""      + statusStr + "\"";
    payload += "}";

    client.publish(topicTelemetry, payload.c_str());
    Serial.println("MQTT -> " + payload);
  }
}
