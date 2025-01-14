#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <UniversalTelegramBot.h>
#define LEDPIN 33
#define FANPIN 25
#define CHAT_ID "969257011"
#define BOTtoken "7141256945:AAHdeNQBtXb0LLy3BvvNIuvje9CFH8vbIUU"

const char* ssid = "Manfred";
const char* password = "Mc--050629";
const char* ESPServerURL = "http://192.168.3.184/data";

bool ledstate = 0;
bool fanstate = 0;
WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

unsigned long lastTimeBotRan = 0;
const unsigned long botRequestDelay = 3000; // 1 second delay between bot updates
bool ledautomation = 1;
bool fanautomation = 1;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi......");
  }
  Serial.println("Connected");
  Serial.println(WiFi.localIP());
  pinMode(LEDPIN, OUTPUT);
  pinMode(FANPIN, OUTPUT);
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
    if (text == "/start") { // Fixed comparison operator
      String welcome = "Welcome, " + from_name + ".\n";
      welcome += "Use the following commands to control the LED:\n\n";
      welcome += "/turn_on_LED \n";
      welcome += "/turn_off_LED \n";
      welcome += "/turn_on_FAN \n";
      welcome += "/turn_off_FAN \n";
      welcome += "/automate \n";
      welcome += "/chart \n";
      if (ledstate == 1) {
        welcome += "LED is on \n";
      }
      else if (ledstate == 0){
        welcome += "LED is off \n";
      }
      if (fanstate == 1) {
        welcome += "FAN is on \n";
      }
      else if (fanstate == 0){
        welcome += "FAN is off \n";
      }
      bot.sendMessage(chat_id, welcome, "");
    }
    if (text == "/turn_on_LED") {
      digitalWrite(LEDPIN, HIGH);
      String message = "LED turned on";
      bot.sendMessage(chat_id, message, "");
      ledautomation = 0;
      ledstate = 1;
    }
    if (text == "/turn_off_LED") {
      digitalWrite(LEDPIN, LOW);
      String message = "LED turned off";
      bot.sendMessage(chat_id, message, "");
      ledautomation = 0;
      ledstate = 0;
    }
    if (text == "/turn_on_FAN") {
      digitalWrite(FANPIN, HIGH);
      String message = "FAN turned on";
      bot.sendMessage(chat_id, message, "");
      fanautomation = 0;
      fanstate = 1;
    }
    if (text == "/turn_off_FAN") {
      digitalWrite(FANPIN, LOW);
      String message = "FAN turned off";
      bot.sendMessage(chat_id, message, "");
      fanautomation = 0;
      fanstate = 0;
    }
    if (text == "/automate") {
      String message = "Initialize automation";
      bot.sendMessage(chat_id, message, "");
      ledautomation, fanautomation = 1 ;
    }
    if (text== "/chart") {
      String message = "https://smart-home-b6i.pages.dev/";
      bot.sendMessage(chat_id, message, "");
    }
  }
}

void handlePins(float t, float l) {
  if (ledautomation == 1) {
    if (l > 2000) {
    digitalWrite(LEDPIN, HIGH);
    ledstate = 1;
    }
    else {
    digitalWrite(LEDPIN, LOW);
    ledstate = 0;
    }
  }
  if (fanautomation == 1){
    if (t > 31) {
    digitalWrite(FANPIN, HIGH);
    fanstate = 1;
    }
    else {
    digitalWrite(FANPIN, LOW);
    fanstate = 0;
    }
  }
  
}

void loop() {
  // put your main code here, to run repeatedly:
  if (millis() > lastTimeBotRan + botRequestDelay) {
    if (ledautomation == 1 || fanautomation == 1) {
      if (WiFi.status() == WL_CONNECTED){
      HTTPClient http;
      http.begin(ESPServerURL);
      int httpcode = http.GET();
      Serial.println(httpcode);
      if (httpcode > 0){
        String payload = http.getString();
        Serial.println("Received: " + payload);
        StaticJsonDocument<200> doc;
        DeserializationError error = deserializeJson(doc, payload);
        if (error) {
        Serial.print("JSON Parsing failed: ");
        Serial.println(error.c_str());
        return;
        }
        float temp = doc["temperature"];
        float LDR = doc["light"];
        Serial.println(temp);
        Serial.println(LDR);
        handlePins(temp, LDR);
      } else {
        Serial.println("Error HTTP");
      } 
      http.end();
      }
    }
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
      while (numNewMessages) {
        Serial.println("New message received");
        handleNewMessages(numNewMessages);
        numNewMessages = bot.getUpdates(bot.last_message_received + 1);
      }
    lastTimeBotRan = millis();    
  }   
}


