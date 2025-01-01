#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#define LEDPIN 33
#define FANPIN 21
const char* ssid = "Manfred";
const char* password = "Mc--050629";

const char* ESPServerURL = "http://192.168.3.184/data";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi......");
  }
  Serial.println("Connected");
  pinMode(LEDPIN, OUTPUT);
  pinMode(FANPIN, OUTPUT);
} 

void handlePins(float t, float l) {
  if (l > 2000) {
    digitalWrite(LEDPIN, HIGH);
  }
  else {
    digitalWrite(LEDPIN, LOW);
  }
  if (t > 31) {
    digitalWrite(FANPIN, HIGH);
  }
  else {
    digitalWrite(FANPIN, LOW);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
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
      };
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
  delay(5500);
}
