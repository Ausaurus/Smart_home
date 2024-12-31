#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <UniversalTelegramBot.h>

const char* ssid = "Manfred";
const char* password = "Mc--050629";

#define CHAT_ID "969257011"
#define BOTtoken "7141256945:AAHdeNQBtXb0LLy3BvvNIuvje9CFH8vbIUU"

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

unsigned long lastTimeBotRan = 0;
const unsigned long botRequestDelay = 1000; // 1 second delay between bot updates
bool automation = 

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
      bot.sendMessage(chat_id, welcome, "");
    }
    if (text == "/turn_on_LED") {
      digitalWrite(18, HIGH);
      String message = "LED turned on";
      bot.sendMessage(chat_id, message, "");
    }
    if (text == "/turn_off_LED") {
      digitalWrite(18, LOW);
      String message = "LED turned off";
      bot.sendMessage(chat_id, message, "");
    }
  }
}

HTTPClient http;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT);
  while (WiFi.status() != WL_CONNECTED){
    delay(1000);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
  pinMode(33,OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  http.begin("http://192.168.3.184/light");
  Serial.println("HTTP Client subscribed to server");
  int httpCode = http.GET();
  if (httpCode > 0) {
    Serial.println(httpCode);
    if (httpCode == HTTP_CODE_OK){
      String payload = http.getString();
      Serial.println(payload);
      if (payload.toInt() > 2800){
        digitalWrite(33, HIGH);
      }
      else {
        digitalWrite(33,LOW);
      }
    }
  }

  delay(30000);
}
