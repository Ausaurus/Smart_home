#include "DHT.h"

// Pin definitions
const int ENA = 14;  // Speed control (PWM)
//const int IN1 = 25;  // Direction control
//const int IN2 = 21;  // Direction control
const int DHTPIN = 13; // Pin connected to DHT22 data pin

// DHT sensor type
#define DHTTYPE DHT22

// PWM settings
const int pwmChannel = 0;
const int pwmFreq = 5000;
const int pwmResolution = 8;  // 8-bit resolution (0-255)

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();

  // Initialize motor control pins
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA, OUTPUT);

  // Set up PWM for ENA pin
  ledcSetup(pwmChannel, pwmFreq, pwmResolution);
  ledcAttachPin(ENA, pwmChannel);

  Serial.println("Motor speed control based on temperature:");
}

void loop() {
  // Read temperature from DHT22
  float temperature = dht.readTemperature();

  // Check if reading is valid
 
  // Determine motor speed based on temperature
  int speed = 0;

  if (temperature <= 15) {
    speed = 100;  // Low speed for <= 15°C
  } else if (temperature <= 25) {
    speed = 150;  // Medium speed for <= 25°C
  } else if (temperature > 30) {
    speed = 255;  // Maximum speed for > 30°C
  }

  // Print temperature and motor speed
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" °C, Motor Speed (PWM): ");
  Serial.println(speed);

  // Move motor forward with calculated speed
  moveForward(speed);

  delay(2000);  // Wait for 2 seconds before next temperature reading
}

// Function to move the motor forward
void moveForward(int speed) {
  digitalWrite(IN1, HIGH);    // Set IN1 HIGH
  digitalWrite(IN2, LOW);     // Set IN2 LOW
  ledcWrite(pwmChannel, speed); // Set PWM duty cycle for ENA
}
