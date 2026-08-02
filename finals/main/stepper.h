#ifndef stepper_h
#define stepper_h

#include <Arduino.h>
#include <FastAccelStepper.h>

extern FastAccelStepperEngine stepperEngine;

class Stepper
{
public:
    Stepper(int stepsPerRotation);
    void begin(FastAccelStepperEngine &engine, uint8_t stepPin, uint8_t dirPin, uint32_t maxSpeed, int32_t maxAcceleration);
    void directTo(float rotation);
    void spinTo(float rotation, int extraRounds);
    void stop();
    bool running();
private:
    long currentPosition();
    int currentRotation();
    FastAccelStepper *stepper_;
    int stepsPerRotation_;
};

class StepperGroup
{
public:
    StepperGroup(Stepper* steppers[], int n);
    void stopAll();
    void allDirectTo(float rotation);
    bool anyRunning() const;
    Stepper* get(int i) const;
private:
    Stepper** steppers_;
    int n_;
};

#endif