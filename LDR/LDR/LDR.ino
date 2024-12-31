#define LDRPIN 34 
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  int val = analogRead(LDRPIN);
  Serial.println("Value: ");
  Serial.println(val);
  delay(500);
}
