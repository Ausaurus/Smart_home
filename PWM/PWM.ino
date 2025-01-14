#define ENA 32        // PWM pin for motor speed control
#define IN1 25         // Motor direction control pin 1

const int freq = 5000;       // PWM frequency in Hz
const int resolution = 8;    // 8-bit resolution (0-255)

void setup() {
  // Set direction pins as outputs
  pinMode(IN1, OUTPUT);

  // Attach PWM to ENA pin with frequency and resolution
  ledcAttach(ENA, freq, resolution);

  // Set initial motor direction
  digitalWrite(IN1, HIGH); // Motor forward
}

void loop() {
  // Gradually increase motor speed
  for (int dutyCycle = 0; dutyCycle <= 255; dutyCycle += 5) {
    ledcWrite(ENA, dutyCycle); // Set duty cycle
    delay(50);                 // Wait 50ms
  }

  // Gradually decrease motor speed
  for (int dutyCycle = 255; dutyCycle >= 0; dutyCycle -= 5) {
    ledcWrite(ENA, dutyCycle); // Set duty cycle
    delay(50);                 // Wait 50ms
  }

  

  // Gradually increase motor speed
  for (int dutyCycle = 0; dutyCycle <= 255; dutyCycle += 5) {
    ledcWrite(ENA, dutyCycle); // Set duty cycle
    delay(50);                 // Wait 50ms
  }


  // Stop the motor for 2 seconds
  digitalWrite(IN1, LOW);
  ledcWrite(ENA, 0); // Set speed to 0
  delay(2000);       // Pause for 2 seconds
}
