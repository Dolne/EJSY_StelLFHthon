#define VIBRATION_PIN 2

void setup() {
  // put your setup code here, to run once:
  pinMode(VIBRATION_PIN, OUTPUT);
}

void loop() {
  digitalWrite(VIBRATION_PIN, 0);
  delay(1000);
  digitalWrite(VIBRATION_PIN, 1);
  delay(1000);
  // put your main code here, to run repeatedly:

}
