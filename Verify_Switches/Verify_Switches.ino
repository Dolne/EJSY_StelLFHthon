#define LED_PIN 26
#define VIBRATION_PIN 27
#define SW1_PIN 22

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_PIN, OUTPUT);
  pinMode(VIBRATION_PIN, OUTPUT);

  pinMode(SW1_PIN, INPUT_PULLUP);
}

void loop() {
  if (digitalRead(SW1_PIN) == 1) {
    digitalWrite(LED_PIN, 1);
    digitalWrite(VIBRATION_PIN, 0);
  }

  else {
    digitalWrite(LED_PIN, 0);
    digitalWrite(VIBRATION_PIN, 1);
  }
  // put your main code here, to run repeatedly:

}
