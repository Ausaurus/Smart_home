#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <WebServer.h>
#include <UniversalTelegramBot.h>
#include <DHT.h>
// WiFi credentials
const char* ssid = "Manfred";
const char* password = "Mc--050629";
const char* apiKey = "YY1BG1HN7M5FAYGK";
// Node.js server address
String serverUrl = "https://api.thingspeak.com/update"; // Replace <your-server-ip> with the Node.js server IP

#define DHTPIN 13
#define LEDPIN 25
#define LDRPIN 34
#define FANPIN 27
#define TELPIN 12
#define BUTTON_PIN 15  // GPIO21 pin connected to button
#define SHORT_PRESS_TIME 5000 // 500 milliseconds
unsigned long lastTimeDataSent = 0;
unsigned long* DataSent_address = &lastTimeDataSent;
const unsigned long ThingSpeakRequestDelay = 7000;
unsigned long lastTimeBotRan = 0;
unsigned long* BotRan = &lastTimeBotRan;
const unsigned long botRequestDelay = 5000;
uint8_t httpResponseCode;
DHT dht(DHTPIN, DHT11);
const char* BOTtoken = "7141256945:AAHdeNQBtXb0LLy3BvvNIuvje9CFH8vbIUU";
const char* CHAT_ID = "969257011";
WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);
int lumens;
float temp;
bool ledAutomation = 1, fanAutomation = 1, ledState = 0, fanState = 0;
bool lastState = 1, TelState = 0, LastTelState = 0;

void handlePins(float*, int*, bool*, bool*, bool*, bool*);
void readDHTTemperature(float*);
void readLightIntensity(int*);
void handleNewMessages(int*, bool*, bool*, bool*, bool*);
void detectButton(bool*, bool*, bool*, bool*, bool*);

void setup() {
  Serial.begin(115200);
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  // Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP());
  pinMode(LEDPIN, OUTPUT);
  pinMode(FANPIN, OUTPUT);
  pinMode(TELPIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
}

void loop() {
  detectButton(&lastState, &TelState, &LastTelState, &ledAutomation, &fanAutomation);
  readDHTTemperature(&temp);
  readLightIntensity(&lumens);
  handlePins(&temp, &lumens, &ledAutomation, &fanAutomation, &ledState, &fanState);
  // Send data to Node.js server
  if (millis() > *DataSent_address + ThingSpeakRequestDelay) {
    HTTPClient http;

    // Combine temperature and light intensity into one request
    String url = serverUrl + "?api_key=" + apiKey +
      "&field1=" + temp +
      "&field2=" + lumens;

    // Serial.println("Generated URL: " + url);
    http.begin(url);

    int httpResponse = http.GET();
    if (httpResponse == HTTP_CODE_OK) {
      // Serial.println("Data sent successfully");
      // Serial.println("Response: " + http.getString());
    } else {
      // Serial.println("Error sending data: " + String(httpResponse));
    }
    http.end();
    *DataSent_address = millis();
  }
  if ((millis() > (lastTimeBotRan + botRequestDelay)) && (TelState)){

    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    Serial.println("Checking new messages from telegram");
    while (numNewMessages) {
      // Serial.println("New message received");
      handleNewMessages(&numNewMessages, &ledAutomation, &fanAutomation, &ledState, &fanState);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();    
  }
}

void handlePins(float* t, int* l, bool* LedAuto, bool* FanAuto, 
    bool* LState, bool* FState) {
  if (*LedAuto) {
    if (*l > 2000) {
      digitalWrite(LEDPIN, HIGH);
      *LState = 1;
    }
    else {
      digitalWrite(LEDPIN, LOW);
      *LState = 0;
    }
  }

  if (*FanAuto) {
    if (*t > 30) {
      digitalWrite(FANPIN, HIGH);
      *FState = 1;
    }
    else {
      digitalWrite(FANPIN, LOW);
      *FState = 0;
    }
  }
}

void readDHTTemperature(float* t) {
  *t = dht.readTemperature();
  if (isnan(*t)) {
    // Serial.println("Failed to read from DHT sensor!");
    return;
  }
}

void readLightIntensity(int* t) {
  *t = analogRead(LDRPIN);
}

void handleNewMessages(int* no_msgs, bool* LedAuto, bool* FanAuto, 
    bool* LState, bool* FState){
  // Serial.println("handleNewMessages");
  // Serial.println(String(*no_msgs));

  for (int i = 0; i < *no_msgs; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID) {
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }
    String text = bot.messages[i].text;
    // Serial.println(text);
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
      if (*LState == 1) {
        welcome += "LED is on \n";
      }
      else if (*LState == 0){
        welcome += "LED is off \n";
      }
      if (*FState == 1) {
        welcome += "FAN is on \n";
      }
      else if (*FState == 0){
        welcome += "FAN is off \n";
      }
      bot.sendMessage(chat_id, welcome, "");
    }
    if (text == "/turn_on_LED") {
      digitalWrite(LEDPIN, HIGH);
      String message = "LED turned on";
      bot.sendMessage(chat_id, message, "");
      *LedAuto = 0;
      *LState = 1;
    }
    if (text == "/turn_off_LED") {
      digitalWrite(LEDPIN, LOW);
      String message = "LED turned off";
      bot.sendMessage(chat_id, message, "");
      *LedAuto = 0;
      *LState = 0;
    }
    if (text == "/turn_on_FAN") {
      digitalWrite(FANPIN, HIGH);
      String message = "FAN turned on";
      bot.sendMessage(chat_id, message, "");
      *FanAuto = 0;
      *FState = 1;
    }
    if (text == "/turn_off_FAN") {
      digitalWrite(FANPIN, LOW);
      String message = "FAN turned off";
      bot.sendMessage(chat_id, message, "");
      *FanAuto = 0;
      *FState = 0;
    }
    // if (text == "/automate") {
    //   String message = "Initialize automation";
    //   bot.sendMessage(chat_id, message, "");
    //   *LedAuto = 1;
    //   *FanAuto = 1;
    // }
    if (text== "/chart") {
      String message = "https://smart-home-b6i.pages.dev/";
      bot.sendMessage(chat_id, message, "");
    }
  }
}

void detectButton(bool* lastState, bool* TelState, bool* LastTelState, bool* LedAuto, bool* FanAuto){
  // read the state of the switch/button:
  int currentState = digitalRead(BUTTON_PIN);
  unsigned long pressedTime = 0, releasedTime = 0;

  if (*lastState == HIGH && currentState == LOW)       // button is pressed
    pressedTime = millis();
  else if (*lastState == LOW && currentState == HIGH) { // button is released
    releasedTime = millis();

    long pressDuration = releasedTime - pressedTime;

    if ( pressDuration < SHORT_PRESS_TIME )
      Serial.println("A short press is detected");
      *TelState = (*LastTelState) ? 0 : 1;
      *LastTelState = *TelState;
      if (*TelState){
        digitalWrite(TELPIN, HIGH);
        // *LedAuto = 0;
        // *FanAuto = 0;
      }
      else {
        digitalWrite(TELPIN, LOW);
        *LedAuto = 1;
        *FanAuto = 1;
      }
  }

  // save the the last state
  *lastState = currentState;
}
