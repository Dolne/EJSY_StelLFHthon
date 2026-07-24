//Based on https://howtomechatronics.com

// defines pins
#define STEP_PIN 2
#define DIR_PIN 5 
//Leave the enable pin unconnected / grounded. If you connect it to 3V3, it will stop working
//The stepper we use is 1.8deg per step but the driver does 8 microsteps per step


void setup() {
  // Sets the two pins as Outputs
  pinMode(STEP_PIN,OUTPUT);
  pinMode(DIR_PIN,OUTPUT);
}

void loop() {
  digitalWrite(DIR_PIN,HIGH);
  turn(5);

  /* // Enables the motor to move in a particular direction
  // Makes 200 pulses for making one full cycle rotation
  for(int x = 0; x < 3200; x++) {
    digitalWrite(STEP_PIN,HIGH); 
    delayMicroseconds(50);    // by changing this time delay between the steps we can change the rotation speed
    digitalWrite(STEP_PIN,LOW);
    delayMicroseconds(50);
  }*/
  delay(3000); // One second delay
  
  /*digitalWrite(DIR_PIN,LOW); //Changes the rotations direction
  // Makes 400 pulses for making two full cycle rotation
  for(int x = 0; x < 1600; x++) {
    digitalWrite(STEP_PIN,HIGH);
    delayMicroseconds(200);
    digitalWrite(STEP_PIN,LOW);
    delayMicroseconds(200);
  }
  delay(1000);*/
}

void turn(int tenthsInput) { //tenthsInput is how many tenths of 360 degrees to turn, accepts >10 too
  int stepsRequired = tenthsInput*160; //1.8 degrees per step + 8 microsteps per step means 160 ((36/1.8)*8=160) steps to turn 36 degrees
  for(int step = 0; step < stepsRequired; step++) {
      digitalWrite(STEP_PIN,HIGH); 
      delayMicroseconds(40);    // by changing this time delay between the steps we can change the rotation speed. Minimum is ~40micro sec
      digitalWrite(STEP_PIN,LOW);
      delayMicroseconds(40);
    }
}