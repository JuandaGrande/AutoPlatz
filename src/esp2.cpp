#include <Arduino.h>
#include <ESP32Servo.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

#define WIFI_SSID       "<Internet SSID>"
#define WIFI_PASSWORD   "<Internet Password>"
#define BOT_TOKEN       "<Telegram_Token>"
#define SECRET_PASSWORD "<Telegram Password>"


#define LED1     19
#define LED2     21
#define LED3     22
#define SENSO1   25   // Occupancy signal from ESP1
#define SENSO2   26   // Occupancy signal from ESP1
#define SENSO3   27   // Occupancy signal from ESP1
#define SENSEXIT 32   // Exit sensor (own)
#define ENTRADA  33   // Authorized card signal from ESP1
#define SERVOIN   4   // Entrance servo
#define SERVOOUT 23   // Exit servo

Servo Servoout;
Servo Servoin;
WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);

volatile bool         OpenToEnter    = false;
volatile unsigned long lastInterrupt = 0;

unsigned long lastBotCheck = 0;
const int     BOT_INTERVAL = 1000; 

unsigned long lastExitDebounce = 0;
const int     EXIT_DEBOUNCE_MS = 500;

void actualizarLEDS();
void openEntrance();
void openExit();
void handleTelegram();
void connectWiFi();

void IRAM_ATTR handleEntrance() {
  unsigned long now = millis();
  if (now - lastInterrupt > 100) {
    OpenToEnter   = true;
    lastInterrupt = now;
  }
}

void setup() {
  Serial.begin(115200);

  // Servos
  Servoin.attach(SERVOIN);
  Servoout.attach(SERVOOUT);

  Serial.println(Servoin.attached()  ? "Servoin OK"  : "Servoin FAILED to attach");
  Serial.println(Servoout.attached() ? "Servoout OK" : "Servoout FAILED to attach");

  // LEDs
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);

  // Sensors
  pinMode(SENSO1,   INPUT_PULLDOWN);
  pinMode(SENSO2,   INPUT_PULLDOWN);
  pinMode(SENSO3,   INPUT_PULLDOWN);
  pinMode(SENSEXIT, INPUT_PULLDOWN);
  pinMode(ENTRADA,  INPUT_PULLDOWN);

  // Interrupt
  attachInterrupt(digitalPinToInterrupt(ENTRADA), handleEntrance, RISING);

  // WiFi + Telegram
  connectWiFi();
  client.setInsecure();

  // Servo self-test
  Servoin.write(0);  Servoout.write(0);  delay(500);
  Servoin.write(90); Servoout.write(90);   delay(500);
  Servoin.write(0);  Servoout.write(0);  delay(500);
  Serial.println("Servos OK — sistema listo.");

  Serial.println("INICIANDO...");
}


void loop() {

  // Reconnect
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi perdido, reconectando...");
    connectWiFi();
  }

  // LEDs
  actualizarLEDS();

  // Exit sensor
  if (digitalRead(SENSEXIT) == LOW && millis() - lastExitDebounce > EXIT_DEBOUNCE_MS) {
    lastExitDebounce = millis();
    Serial.println("DETECTADO SALIDA");
    openExit();
  }

  // Entrance (With Interrupt)
  if (OpenToEnter) {
    OpenToEnter = false;
    openEntrance();
  }

  // Telegram polling
  handleTelegram();
}

void connectWiFi() {
  Serial.print("Conectando a WiFi");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi conectado. IP: " + WiFi.localIP().toString());
  } else {
    Serial.println("\nNo se pudo conectar al WiFi. Continuando sin internet.");
  }
}

void handleTelegram() {
  if (millis() - lastBotCheck < BOT_INTERVAL) return;
  lastBotCheck = millis();


  if (WiFi.status() != WL_CONNECTED) return;

  int numMessages = bot.getUpdates(bot.last_message_received + 1);

  while (numMessages) {
    for (int i = 0; i < numMessages; i++) {
      String text     = bot.messages[i].text;
      String chat     = bot.messages[i].chat_id;
      String fromName = bot.messages[i].from_name;

      Serial.println("Mensaje de " + fromName + " (" + chat + "): " + text);

      String validCommand = "/abrir " + String(SECRET_PASSWORD);

      if (text == validCommand) {

        bot.sendMessage(chat, "✅ Acceso autorizado. Abriendo entrada...", "");
        Serial.println("Apertura remota por Telegram — " + fromName);
        openEntrance();

      } else if (text.startsWith("/abrir")) {
        bot.sendMessage(chat, "❌ Contraseña incorrecta. Acceso denegado.", "");
        Serial.println("Intento fallido desde: " + fromName);

      } else if (text == "/estado") {
        String estado = "📊 Estado del parqueadero:\n";
        estado += digitalRead(SENSO1) ? "🔴 Espacio 1: Ocupado\n"   : "🟢 Espacio 1: Libre\n";
        estado += digitalRead(SENSO2) ? "🔴 Espacio 2: Ocupado\n"   : "🟢 Espacio 2: Libre\n";
        estado += digitalRead(SENSO3) ? "🔴 Espacio 3: Ocupado"     : "🟢 Espacio 3: Libre";
        bot.sendMessage(chat, estado, "");

      } else {
        bot.sendMessage(chat,
          "ℹ️ Comandos disponibles:\n"
          "/abrir <contraseña> — Abrir entrada\n"
          "/estado — Ver espacios disponibles", "");
      }
    }
    numMessages = bot.getUpdates(bot.last_message_received + 1);
  }
}

void openEntrance() {
  Servoin.write(90);
  delay(3000);
  Servoin.write(0);
  Serial.println("ENTRADA");
}

void openExit() {
  Servoout.write(90);
  delay(3000);
  Servoout.write(0);
  Serial.println("SALIDA");
}

void actualizarLEDS() {
  digitalWrite(LED1, digitalRead(SENSO1));
  digitalWrite(LED2, digitalRead(SENSO2));
  digitalWrite(LED3, digitalRead(SENSO3));
}
