#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>
// WiFi credentials
const char* ssid = "Manfred";
const char* password = "Mc--050629";

// Node.js server address
const char* serverUrl = "https://view-regularly-electronics-circuit.trycloudflare.com/update"; // Replace <your-server-ip> with the Node.js server IP

#define DHTPIN 13
#define LDRPIN 34

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

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP());
}

void loop() {

  // Send data to Node.js server
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/json");

    // Create JSON payload
    String payload = "{\"temperature\":" + readDHTTemperature() + ",\"light\":" + readLightIntensity() + "}";
    int httpResponseCode = http.POST(payload);

    if (httpResponseCode > 0) {
      Serial.println("Data sent: " + payload);
      Serial.println("Response: " + http.getString());
    } else {
      Serial.println("Error sending data: " + String(httpResponseCode));
    }

    http.end();
  }

  delay(5000); // Wait 5 seconds before sending again
}

