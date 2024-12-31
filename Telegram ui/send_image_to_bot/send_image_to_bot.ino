#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#define CHAT_ID "969257011"
#define BOT_TOKEN "7141256945:AAHdeNQBtXb0LLy3BvvNIuvje9CFH8vbIUU"

const char* ssid = "Manfred";
const char* password = "Mc--050629";

const unsigned long BOT_MTBS = 1000; // mean time between scan messages

unsigned long bot_lasttime = 0;          // last time messages' scan has been done
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);

String test_photo_url = "https://thingspeak.com/channels/2788337/fields/1.png";

void handleNewMessages(int numNewMessages) {
  Serial.print("handleNewMessages ");
  Serial.println(String(numNewMessages));

  for (int i=0; i<numNewMessages; i++) {
    String chat_id = bot.messages[i].chat_id;
    String text = bot.messages[i].text;

    String from_name = bot.messages[i].from_name;
    if (text == "/get_test_photo") {
      bot.sendMessage(chat_id, test_photo_url, "");
    }

    if (text == "/start") {
      String welcome = "Welcome to Universal Arduino Telegram Bot library, " + from_name + ".\n";
      welcome += "This is Send Image From URL example.\n\n";
      welcome += "/get_test_photo : getting test photo\n";

      bot.sendMessage(chat_id, welcome, "");
    }
  }
}

void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.print("Connecting to Wifi SSID ");
  Serial.print(ssid);
  WiFi.begin(ssid, password);  
  secured_client.setCACert(TELEGRAM_CERTIFICATE_ROOT);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.print("\nWiFi connected. IP address: ");
  Serial.println(WiFi.localIP());
}

void loop()
{
  if (millis() > bot_lasttime + BOT_MTBS)
  {
    Serial.println("Checking for new messages...");
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages)
    {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    bot_lasttime = millis();
  }
}
