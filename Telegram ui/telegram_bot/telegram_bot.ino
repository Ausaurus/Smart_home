#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

// WiFi credentials
const char* ssid = "Manfred";
const char* password = "Mc--050629";

// Telegram bot details
#define CHAT_ID "969257011"
#define BOTtoken "7141256945:AAHdeNQBtXb0LLy3BvvNIuvje9CFH8vbIUU"

// Telegram bot objects
WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

// Global variables
unsigned long lastTimeBotRan = 0;
const unsigned long botRequestDelay = 1000; // 1 second delay between bot updates

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

void setup() {
  Serial.begin(115200); // Fixed missing semicolon
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  // Ensure TELEGRAM_CERTIFICATE_ROOT is included or defined
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  
  Serial.println(WiFi.localIP());
  pinMode(18, OUTPUT); // Initialize LED pin
}

void loop() {
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
