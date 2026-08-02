#include <Arduino.h>
#include <FastAccelStepper.h>

const uint8_t STEP_PINS[] = { 26, 27, 32, 33 };
const uint8_t DIR_PINS[] = { 12, 13, 14, 15 };

FastAccelStepperEngine engine;

FastAccelStepper *steppers[4];

void setup()
{
    Serial.begin(9600);
    for (int i = 0; i < 4; i++) 
    {
        // TODO try FasDriver::RMT and FasDriver::MCPWM_PCNT
        steppers[i] = engine.stepperConnectToPin(STEP_PINS[i], FasDriver::RMT);
        if (steppers[i]) 
        {
            steppers[i]->setDirectionPin(DIR_PINS[i]);
            steppers[i]->setSpeedInHz(3000);
            steppers[i]->setAcceleration(800);
            steppers[i]->moveTo(1600 * (i + 3));
        } 
        else 
        {
            Serial.print("FAILED TO CREATE STEPPER ");
            Serial.println(i + 1);
        }
    }
}

void loop()
{
    // block to the loop to ensure steppers are running off the cpu
    static int t = 0;
    delay(1000);
    Serial.println(++t);
}