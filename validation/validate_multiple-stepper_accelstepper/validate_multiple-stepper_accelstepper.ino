/*
reference: https://hackaday.io/project/183713/instructions
https://hackaday.io/project/183279-accelstepper-the-missing-manual

The motion functions are of two types: constant speed (limited by the current value of speed), and variable speed (limited by settings of acceleration and maxSpeed, and by position relative to the target). Each of these types contains blocking and non-blocking functions. Blocking functions will run until a stop condition occurs, but no other code will run until they complete. Non-blocking functions cause a single step (if a step is due - see below) and return immediately in any case. Since each call to a non-blocking function only makes at most a single step, they must be called as often as possible; usually in the main loop. Otherwise the motor will not be stepped at the desired speed.

The function runSpeed() determines when a step is to be taken. runSpeed() subtracts the time of the last step from the current time. If the result is greater than or equal to the value of stepInterval ( we say, "A step is due."), runSpeed will increment (or decrement, as required) currentPosition, call step(), and update the time the last step was taken. (step() is an internal function - not directly callable. It causes the correct electrical signal to be sent to the interface.) Each call to runSpeed() repeats this process, so runSpeed() must be called at least speed times per second. Usually, this means putting a call to runSpeed() in loop(). Calling runSpeed() when no step is due does nothing

"Ok, fine," you say. "I get constant speed, but how does a motor accelerate or run to a position?" Good question! To do both of those, the run() function is used. The run() function first calls runSpeed() to cause a step (if it is due, as defined above) at the current speed and direction. Then it calls computeNewSpeed(). The function computeNewSpeed() computes a new speed (stepInterval) and sets that as the current speed.

*/


// Bounce.pde
// -*- mode: C++ -*-
//
// Make a single stepper bounce from one limit to another
//
// Copyright (C) 2012 Mike McCauley
// $Id: Random.pde,v 1.1 2011/01/05 01:51:01 mikem Exp mikem $

// defines pins
#define A_STEP_PIN 26
#define A_DIR_PIN 12

#define B_STEP_PIN 27
#define B_DIR_PIN 13

#define C_STEP_PIN 32
#define C_DIR_PIN 14

#define D_STEP_PIN 33
#define D_DIR_PIN 15


//Leave the enable pin unconnected / grounded. If you connect it to 3V3, it will stop working
//The stepper we use is 1.8deg per step but the driver does 8 microsteps per step
//1.8 degrees per step & 8 microsteps per step means 1600 ((360/1.8)*8=1600) steps to turn 360 degrees

#include <AccelStepper.h>
/*#include <MultiStepper.h>
https://www.airspayce.com/mikem/arduino/AccelStepper/classMultiStepper.html#details
don't need use multistepper since multistepper is more to cause all the steppers to move to selected positions at such a (constant) speed that they all arrive at their target position at the same time. This can be used to support devices with multiple steppers on say multiple axes to cause linear diagonal motion. Suitable for use with X-Y plotters, flatbeds, 3D printers etc to get linear straight line movement between arbitrary 2d (or 3d or ...) positions.*/


// Define steppers and the pins it will use
AccelStepper stepperA(AccelStepper::DRIVER, A_STEP_PIN, A_DIR_PIN);
AccelStepper stepperB(AccelStepper::DRIVER, B_STEP_PIN, B_DIR_PIN);
AccelStepper stepperC(AccelStepper::DRIVER, C_STEP_PIN, C_DIR_PIN);
AccelStepper stepperD(AccelStepper::DRIVER, D_STEP_PIN, D_DIR_PIN);

void setup()
{  
  // Change these to suit your stepper if you want
  stepperA.setMaxSpeed(3000); //Approximate based on basic testing of validate_stepper script. Originally 12000, quartered to reduce current draw
  stepperB.setMaxSpeed(3000);
  stepperC.setMaxSpeed(3000);
  stepperD.setMaxSpeed(3000);

  stepperA.setAcceleration(200); //Expensive call, calcs a sqrt. Originally 800, quartered to reduce current draw
  stepperB.setAcceleration(200);
  stepperC.setAcceleration(200);
  stepperD.setAcceleration(200);

  stepperA.moveTo(160000); //100 full rotations
  stepperB.moveTo(160000); //100 full rotations
  stepperC.moveTo(160000);
  stepperD.moveTo(160000);

  //setCurrentPosition((long) position) this function does homing
}

void loop()
{
  // If at the end of travel go to the other end
  if (stepperA.distanceToGo() == 0) {
    stepperA.moveTo(-stepperA.currentPosition());
  }
  if (stepperB.distanceToGo() == 0) {
    stepperB.moveTo(-stepperB.currentPosition());
  }
  if (stepperC.distanceToGo() == 0) {
    stepperC.moveTo(-stepperC.currentPosition());
  }
  if (stepperD.distanceToGo() == 0) {
    stepperD.moveTo(-stepperD.currentPosition());
  }

  
  stepperA.run();
  stepperB.run();
  stepperC.run();
  stepperD.run();
}
