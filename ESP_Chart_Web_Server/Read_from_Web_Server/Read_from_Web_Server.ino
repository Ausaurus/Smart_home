#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <UniversalTelegramBot.h>

const char* ssid = "Manfred";
const char* password = "Mc--050629";

#define CHAT_ID "969257011"
#define BOTtoken "7141256945:AAHdeNQBtXb0LLy3BvvNIuvje9CFH8vbIUU"

const char* caCert = R"EOF(-----BEGIN CERTIFICATE-----
MIID5DCCA4qgAwIBAgIQDwb6DvaWvE4N5hRIilfqkzAKBggqhkjOPQQDAjA7MQsw
CQYDVQQGEwJVUzEeMBwGA1UEChMVR29vZ2xlIFRydXN0IFNlcnZpY2VzMQwwCgYD
VQQDEwNXRTEwHhcNMjQxMjMxMDUwNjU4WhcNMjUwMzMxMDYwMzI3WjArMSkwJwYD
VQQDEyBtYW5mcmVkY2hlb25neXVzaGVhbi53b3JrZXJzLmRldjBZMBMGByqGSM49
AgEGCCqGSM49AwEHA0IABOosq7yb8Ov1wRgZX1JLFSsD6iOjTtqjKuhhwNzfgX/B
LNVoTUL1rTh15LC4+3r5I+z6MODOJZia5OQyqVauFhejggJ+MIICejAOBgNVHQ8B
Af8EBAMCB4AwEwYDVR0lBAwwCgYIKwYBBQUHAwEwDAYDVR0TAQH/BAIwADAdBgNV
HQ4EFgQUkkOQ1Km2Xr164TT7RNb616vJzdgwHwYDVR0jBBgwFoAUkHeSNWfE/6jM
qeZ72YB5e8yT+TgwXgYIKwYBBQUHAQEEUjBQMCcGCCsGAQUFBzABhhtodHRwOi8v
by5wa2kuZ29vZy9zL3dlMS9Ed1kwJQYIKwYBBQUHMAKGGWh0dHA6Ly9pLnBraS5n
b29nL3dlMS5jcnQwTwYDVR0RBEgwRoIgbWFuZnJlZGNoZW9uZ3l1c2hlYW4ud29y
a2Vycy5kZXaCIioubWFuZnJlZGNoZW9uZ3l1c2hlYW4ud29ya2Vycy5kZXYwEwYD
VR0gBAwwCjAIBgZngQwBAgEwNgYDVR0fBC8wLTAroCmgJ4YlaHR0cDovL2MucGtp
Lmdvb2cvd2UxL3JnZW9rUkFfbi1nLmNybDCCAQUGCisGAQQB1nkCBAIEgfYEgfMA
8QB3AE51oydcmhDDOFts1N8/Uusd8OCOG41pwLH6ZLFimjnfAAABlBtTGvYAAAQD
AEgwRgIhAKG2xSR4kX4MSF7vzoeaClkGC3mDEnlvbFHnSquEeDCLAiEAokwLhlFo
RLLLSNlb/iwMT2O/W8nOZsVB33c+MFOJnHwAdgDm0jFjQHeMwRBBBtdxuc7B0kD2
loSG+7qHMh39HjeOUAAAAZQbUxsDAAAEAwBHMEUCIC+834vGzgUZs0dgbVzI/h+u
kbwjxdgQX631dvGrjkVBAiEA+pbiYkN03GVktKEZDu5JS2woTGNDRgbF5BVaRF5u
RYkwCgYIKoZIzj0EAwIDSAAwRQIhAL2GVvFjKOWga7+xRiywBMtxUffzP8YVfBjI
E1OeogxfAiAfSXEN4JvdNe1htgYt5+8BKJ3rnbr6BMG66O2zXXdGow==
-----END CERTIFICATE-----)EOF";

WiFiClientSecure telegramClient; // For Telegram Bot
WiFiClientSecure httpClient;     // For HTTP requests
UniversalTelegramBot bot(BOTtoken, telegramClient);
// unsigned long lastTimeBotRan = 0;
// const unsigned long botRequestDelay = 1000; // 1 second delay between bot updates

/*void handleNewMessages(int numNewMessages) {
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
}*/

HTTPClient http;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  telegramClient.setCACert(TELEGRAM_CERTIFICATE_ROOT);
  httpClient.setCACert(caCert);
  while (WiFi.status() != WL_CONNECTED){
    delay(1000);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
  pinMode(33,OUTPUT);
  StaticJsonDocument<200> data;
}

void loop() {
  // put your main code here, to run repeatedly:
  
  http.begin(httpClient, "https://send-json-data.manfredcheongyushean.workers.dev/");

  Serial.println("HTTP Client subscribed to server");
  int httpCode = http.GET();
  if (httpCode > 0) {
    Serial.println(httpCode);
    if (httpCode == HTTP_CODE_OK){
      String payload = http.getString();
      Serial.println(payload);
    }
  }
  delay(30000);
}
