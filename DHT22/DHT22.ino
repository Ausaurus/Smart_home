#include <DHT.h>
#include <WiFi.h>
#include <HTTPClient.h>
const int DHTPIN = 13;
DHT dht(DHTPIN, DHT11);
int room_temp = 0;
const char* ssid = "Manfred";
const char* pass = "Mc--050629";
String message;              // Variable to store the HTTP request message

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200); 
  dht.begin();
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  // put your main code here, to run repeatedly:
  float temperature = dht.readTemperature();
  Serial.print(temperature);
  Serial.print("\n");
    // Create a message for ThingSpeak including sensor data and statuses
  
  // Send the HTTP request to update ThingSpeak with the data
  delay(14000);
}
