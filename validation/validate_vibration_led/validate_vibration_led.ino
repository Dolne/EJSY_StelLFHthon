#define LED_PIN 26
#define VIBRATION_PIN 27

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_PIN, OUTPUT);
  pinMode(VIBRATION_PIN, OUTPUT);
}

void loop() {
  digitalWrite(LED_PIN, 1);
  digitalWrite(VIBRATION_PIN, 0);
  delay(1000);
  digitalWrite(LED_PIN, 0);
  digitalWrite(VIBRATION_PIN, 1);
  delay(1000);
  // put your main code here, to run repeatedly:

}
