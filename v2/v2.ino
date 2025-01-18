#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <UniversalTelegramBot.h>

// Pin Definitions
#define LEDPIN 33

// Motor Control Pins
const int ENA = 14;
const int IN1 = 25;
const int IN2 = 21;

// PWM Settings
const int pwmChannel = 0;
const int pwmFreq = 5000;
const int pwmResolution = 8;

const char* ssid = "Lianbushangshizhu";
const char* password = "Klwk0405";
const char* ESPServerURL = "https://api.thingspeak.com/channels/2788337/feeds.json?results=1";
const char* BOTtoken = "7141256945:AAHdeNQBtXb0LLy3BvvNIuvje9CFH8vbIUU";
const char* CHAT_ID = "969257011";

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

unsigned long lastTimeBotRan = 0;
const unsigned long botRequestDelay = 6000;
unsigned long lastTimeCheck = 0;
const unsigned long checkRequestDelay = 17000;

bool ledState = false;
bool fanState = false;
bool ledAutomation = true;
bool fanAutomation = true;

void setup() {
  Serial.begin(115200);

  // Initialize WiFi
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Initialize Motor Control Pins
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA, OUTPUT);
  ledcSetup(pwmChannel, pwmFreq, pwmResolution);
  ledcAttachPin(ENA, pwmChannel);
  
  // Set LED as Output
  pinMode(LEDPIN, OUTPUT);

  Serial.println("System Initialized");
}

void handlePins(float t, float l) {
  if (ledAutomation) {
    if (l > 2000) {
      digitalWrite(LEDPIN, HIGH);
      ledState = 1;
    } else {
      digitalWrite(LEDPIN, LOW);
      ledState = 0;
    }
  }
  
  if (fanAutomation) {
    controlMotorBasedOnTemperature(t);
    fanState = 1;
  }
}

void controlMotorBasedOnTemperature(float temperature) {
  int pwmSpeed = (temperature < 15) ? 150 : (temperature <= 25) ? 200 : 255;
  moveMotor(pwmSpeed, true);
}

void moveMotor(int speed, bool forward) {
  digitalWrite(IN1, forward ? HIGH : LOW);
  digitalWrite(IN2, forward ? LOW : HIGH);
  ledcWrite(pwmChannel, speed);
}

void stopMotor() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  ledcWrite(pwmChannel, 0);
}

void loop() {
  if (ledAutomation == 1 || fanAutomation == 1) {
    if (WiFi.status() == WL_CONNECTED){
      if (millis() > lastTimeCheck + checkRequestDelay) {
        HTTPClient http;
        http.begin(ESPServerURL);
        int httpCode = http.GET();
        Serial.println(httpCode);
        if (httpCode > 0) {
          String payload = http.getString();
          Serial.println("Received: " + payload);

          StaticJsonDocument<1024> doc;
          DeserializationError error = deserializeJson(doc, payload);
          if (error) {
            Serial.print("JSON Parsing failed: ");
            Serial.println(error.c_str());
            return;
          }

          JsonObject feed = doc["feeds"][0]; // Extract the first feed
          float lightIntensity = feed["field2"].as<float>(); // Extract field1
          float temperature = feed["field1"].as<float>();    // Extract field2

          Serial.print("Light Intensity: ");
          Serial.println(lightIntensity);

          Serial.print("Temperature: ");
          Serial.println(temperature);

          handlePins(temperature, lightIntensity);
        } else {
          Serial.println("Error HTTP");
        } 
        http.end();
        lastTimeCheck = millis();
      }
    }
  }
  if (millis() > lastTimeBotRan + botRequestDelay) {
    
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    while (numNewMessages) {
      Serial.println("New message received");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();    
  }   
}

void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));
  
  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID) {
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }
    String text = bot.messages[i].text;
    Serial.println(text);
    String from_name = bot.messages[i].from_name;
    if (text == "/start") {
      String welcome = "Welcome, " + from_name + ".\n";
      welcome += "Use the following commands to control the LED:\n\n";
      welcome += "/turn_on_LED \n";
      welcome += "/turn_off_LED \n";
      welcome += "/turn_on_FAN \n";
      welcome += "/turn_off_FAN \n";
      welcome += "/automate \n";
      welcome += "/chart \n";
      if (ledState == 1) {
        welcome += "LED is on \n";
      } else if (ledState == 0){
        welcome += "LED is off \n";
      }
      if (fanState == 1) {
        welcome += "FAN is on \n";
      } else if (fanState == 0){
        welcome += "FAN is off \n";
      }
      bot.sendMessage(chat_id, welcome, "");
    }
    if (text == "/turn_on_LED") {
      digitalWrite(LEDPIN, HIGH);
      String message = "LED turned on";
      bot.sendMessage(chat_id, message, "");
      ledAutomation = 0;
      ledState = 1;
    }
    if (text == "/turn_off_LED") {
      digitalWrite(LEDPIN, LOW);
      String message = "LED turned off";
      bot.sendMessage(chat_id, message, "");
      ledAutomation = 0;
      ledState = 0;
    }
    if (text == "/turn_on_FAN") {
      controlMotorBasedOnTemperature(30);
      String message = "FAN turned on";
      bot.sendMessage(chat_id, message, "");
      fanAutomation = 0;
      fanState = 1;
    }
    if (text == "/turn_off_FAN") {
      stopMotor();
      String message = "FAN turned off";
      bot.sendMessage(chat_id, message, "");
      fanAutomation = 0;
      fanState = 0;
    }
    if (text == "/automate") {
      String message = "Initialize automation";
      bot.sendMessage(chat_id, message, "");
      ledAutomation = 1;
      fanAutomation = 1;
    }
    if (text == "/chart") {
      String message = "https://smart-home-b6i.pages.dev/";
      bot.sendMessage(chat_id, message, "");
    }
  }
}
