const int buttonUpPin = 19;
const int buttonSelectPin = 18;
const int buttonDownPin = 17; //Starts at 17 because pin 16 is connected to the ESP's onboard neopixel

void setup() {
  // put your setup code here, to run once:
  pinMode(buttonUpPin, INPUT_PULLUP); //the switches are normally closed and will be connected to ground
  pinMode(buttonSelectPin, INPUT_PULLUP);
  pinMode(buttonDownPin, INPUT_PULLUP);
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly
  Serial.print(digitalRead(buttonUpPin));
  Serial.print(digitalRead(buttonSelectPin));
  Serial.println(digitalRead(buttonDownPin));
}
