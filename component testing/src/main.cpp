#include <Arduino.h>
/* Example sketch to control a stepper motor with 
   DRV8825 stepper motor driver, AccelStepper library 
   and Arduino: continuous rotation. 
   More info: https://www.makerguides.com */

#include "AccelStepper.h"

// Define stepper motor connections and motor interface type. Motor interface type must be set to 1 when using a driver:
#define dirPin 12
#define stepPin 13
#define motorInterfaceType 1

// Create a new instance of the AccelStepper class:
AccelStepper stepper = AccelStepper(motorInterfaceType, stepPin, dirPin);

// motor config (adjust to your motor/driver)
const long motorStepsPerRev = 200;      // full steps per revolution for your motor
// const int microstepsPerStep = 8;      // microsteps set on driver (1,2,4,8,16,...)

int generateTurn(int anglePos){
  int rounds = random(3);
  // long current = stepper.currentPosition();
  int turns = (rounds * 360 + anglePos) * motorStepsPerRev;
  return turns;
}


void setup() {
  // Set the maximum speed in steps per second:
  Serial.begin(115200);
  stepper.setMaxSpeed(2500);
  stepper.setAcceleration(30);
  stepper.setCurrentPosition(0);
  delay(2000);
  stepper.moveTo((3 * 360 + 180) * motorStepsPerRev / 360);
};

void loop() {
  stepper.run();
  // Serial.println(stepper.currentPosition());
  // Set the speed in steps per second:
  // stepper.setSpeed(400);
  // generateTurn(180);
  // Step the motor with a constant speed as set by setSpeed():
  // stepper.runSpeed();
}