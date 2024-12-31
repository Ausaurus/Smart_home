#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "Manfred";
const char* pass = "Mc--050629";
String apiKey = "YY1BG1HN7M5FAYGK";
String url = "https://api.thingspeak.com/update";

String message;              // Variable to store the HTTP request message
uint8_t httpCode;            // Variable to store HTTP response code
HTTPClient http;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  // Create a message for ThingSpeak including sensor data and statuses
  message = url + "?api_key=" + apiKey + "&field1=" + String(1, 2);
  
  // Send the HTTP request to update ThingSpeak with the data
  http.begin(message);
  Serial.println(message);
  httpCode = http.GET();
  if (httpCode == HTTP_CODE_OK) {
    message = http.getString();
    Serial.println(message);
  }
  http.end();
  // Delay for 7 seconds before the next loop iteration
  delay(1500);
}

