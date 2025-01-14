#include <WiFi.h>
#include <HTTPClient.h>
#include <WebServer.h>
#include <DHT.h>
// WiFi credentials
const char* ssid = "Manfred";
const char* password = "Mc--050629";
const char* apiKey = "YY1BG1HN7M5FAYGK";
// Node.js server address
String serverUrl = "https://api.thingspeak.com/update"; // Replace <your-server-ip> with the Node.js server IP

#define DHTPIN 13
#define LDRPIN 34
unsigned long lastTimeDataSent = 0;
const unsigned long ThingSpeakRequestDelay = 17000;
uint8_t httpResponseCode;
DHT dht(DHTPIN, DHT11);
WebServer server(80);

String readDHTTemperature() {
  float t = dht.readTemperature();
  if (isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return "";
  }
  Serial.print("Temperature");
  Serial.println(t);
  return String(t);
}

String readLightIntensity() {
  int t = analogRead(LDRPIN);
  Serial.print("light intensity");
  Serial.println(t);
  return String(t);
}

void handleData(){
  String payload = "{\"temperature\":" + readDHTTemperature() + ",\"light\":" + readLightIntensity() + "}";
  server.send(200, "application/json", payload);
  Serial.println("sent");
}

void setup() {
  Serial.begin(115200);
   
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP());
  server.on("/data", handleData);
  server.begin();
}

void loop() {
  server.handleClient();
  // Send data to Node.js server
  if (millis() > lastTimeDataSent + ThingSpeakRequestDelay) {
    HTTPClient http;

    // Combine temperature and light intensity into one request
    String url = serverUrl + "?api_key=" + apiKey +
                 "&field1=" + readDHTTemperature() +
                 "&field2=" + readLightIntensity();

    Serial.println("Generated URL: " + url);
    http.begin(url);

    int httpResponse = http.GET();
    if (httpResponse == HTTP_CODE_OK) {
      Serial.println("Data sent successfully");
      Serial.println("Response: " + http.getString());
    } else {
      Serial.println("Error sending data: " + String(httpResponse));
    }
    http.end();

    lastTimeDataSent = millis();
  }

  delay(3000); // Wait 5 seconds before sending again
}

