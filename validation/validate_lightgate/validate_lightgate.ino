#define LIGHT_GATE_PIN 25 //Will go high (1) when it's blocked, is low (0) when not blocked

void setup() {
  // put your setup code here, to run once:
  pinMode(LIGHT_GATE_PIN, INPUT);
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(digitalRead(LIGHT_GATE_PIN));
}
