// Include necessary libraries for the ESP32 project
#include "soc/soc.h"          // Library for disabling brownout detector
#include "soc/rtc.h"          // Library for configuring the RTC (Real-Time Clock)
#include <WiFi.h>             // Library for WiFi connection
#include <HTTPClient.h>       // Library for making HTTP requests
#include <DHT.h>              // Library for DHT22 sensor
#include <ESP32Servo.h>      // Library for controlling a servo motor with ESP32

// Declare WiFi SSID and password for connecting to a network
const char* ssid = "wifi name";
const char* password = "wifi password";

// Declare API key for ThingSpeak, a platform for IoT data collection
String apiKey = "API Key";
String url = "https://api.thingspeak.com/update";

String message;              // Variable to store the HTTP request message
uint8_t httpCode;            // Variable to store HTTP response code

// Declare pin connected to the servo's signal pin
static const int servoPin = 13;

// Declare pin connected to DHT22 sensor's data pin
#define DHTPIN 32

// Create instances of the Servo and DHT sensor libraries
Servo servo1;
DHT dht(DHTPIN, DHT11);
HTTPClient http;

int angle = 0;
int angleStep = 5;
int angleMin = 0;
int angleMax = 180;
int soil_moisture = 50;
int pump_status, alarm_status;
int temp1 = 0;

void setup() {
    // Disable brownout detector to prevent resets
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
    
    // Initialize serial communication for debugging
    Serial.begin(115200);

    // Initialize DHT sensor
    dht.begin();

    // Attach the servo to its signal pin
    servo1.attach(servoPin);

    // Set pin 2 as an output for controlling a device
    pinMode(2, OUTPUT);

    // Connect to the specified WiFi network
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");
}

void loop() {
  // Read temperature and humidity from the DHT sensor
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  
  // Print temperature to the serial monitor
  Serial.print("temperature: ");
  Serial.println(temperature);

  // Adjust soil moisture level based on temperature
  if (temperature > 30) {
    soil_moisture = soil_moisture - 10;
  }

  // Check soil moisture level and control the pump and alarm
  if (soil_moisture <= 20) {
    alarm_status = 1;
    pump_status = 1;
    digitalWrite(2, HIGH);
    pumpOn();
    Serial.println("PUMP ON");
  } else if (soil_moisture > 20) {
    alarm_status = 0;
    pump_status = 0;
    digitalWrite(2, LOW);
    pumpOff();
    Serial.println("PUMP OFF");
  }
  
  // Create a message for ThingSpeak including sensor data and statuses
  message = url + "?api_key=" + apiKey + "&field1=" + String(temperature, 2) + "&field2=" + String(soil_moisture) + "&field3=" + String(alarm_status) + "&field4=" + String(pump_status);
  
  // Send the HTTP request to update ThingSpeak with the data
  http.begin(message);
  Serial.println(message);
  httpCode = http.GET();
  if (httpCode == HTTP_CODE_OK) {
    message = http.getString();
    Serial.println(message);
  }
  
  // Restore soil moisture level if necessary
  if (soil_moisture <= 20) {
    soil_moisture = soil_moisture + 10;
  }
  
  // Delay for 7 seconds before the next loop iteration
  delay(7000);
}

// Function to turn off the pump (move the servo to 0 degrees)
void pumpOff() {
    servo1.write(0);
    delay(20);
}

// Function to turn on the pump (move the servo to 90 degrees)
void pumpOn() {
    servo1.write(90);
    Serial.println(90);
    delay(20);
}
