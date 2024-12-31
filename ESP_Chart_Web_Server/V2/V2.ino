#include <WiFi.h>
#include <WebServer.h>
#include <LittleFS.h>
#include <DHT.h>

#define DHTPIN 13
#define LDRPIN 34
const char* ssid = "Manfred";
const char* password = "Mc--050629";

WebServer server(80);
DHT dht(DHTPIN, DHT11);

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

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP());
  if (!LittleFS.begin()) {
    Serial.println("Failed to mount LittleFS");
    return;
  }

  server.serveStatic("/", LittleFS, "/index.html");

  server.on("/temperature", HTTP_GET, []() {
    String temperature = readDHTTemperature();
    server.send(200, "text/plain", temperature.c_str());
  });

  server.on("/light", HTTP_GET, []() {
    String light = readLightIntensity();
    server.send(200, "text/plain", light.c_str());
  });

  server.begin();
}

void loop() {
  server.handleClient();
}
