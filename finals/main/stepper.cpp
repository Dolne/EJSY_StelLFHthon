#include "stepper.h"

int posMod(long num, int by) {
    int val = num % by;
    if (val < 0) {
        return val + by;
    }
    return val;
}

FastAccelStepperEngine stepperEngine = FastAccelStepperEngine();

Stepper::Stepper(int stepsPerRotation):
    stepsPerRotation_(stepsPerRotation)
{
}

void Stepper::begin(FastAccelStepperEngine &engine, uint8_t stepPin, uint8_t dirPin, uint32_t maxSpeed, int32_t maxAcceleration)
{
    stepper_ = engine.stepperConnectToPin(stepPin, FasDriver::RMT);
    if (stepper_ == nullptr) {
        Serial.println("FAILED TO CREATE STEPPER");
        return;
    }
    stepper_->setDirectionPin(dirPin);
    stepper_->setSpeedInHz(maxSpeed);
    stepper_->setAcceleration(maxAcceleration);
}

// this may not work very well if stepper is moving
void Stepper::directTo(float rotation)
{
    if (stepper_) {
        stepper_->stopMove(); // use this to get the minimum distance will stepper must go if it is moving
        int32_t startAbs = stepper_->targetPos();
        int start = posMod(startAbs, stepsPerRotation_);
        int diff1 = posMod(long(rotation * stepsPerRotation_), stepsPerRotation_) - start;
        int diff2 = diff1 < 0 ? diff1 + stepsPerRotation_ : diff1 - stepsPerRotation_;

        // move the shorter distance to the target rotation
        Serial.print("direct to ");
        if (abs(diff1) <= abs(diff2)) {
            Serial.println(startAbs + diff1);
            stepper_->moveTo(startAbs + diff1);
        } else {
            Serial.println(startAbs + diff2);
            stepper_->moveTo(startAbs + diff2);
        }
    }
}

// this may not work very well if stepper is moving
void Stepper::spinTo(float rotation, int extraRounds)
{
    if (stepper_) {
        int diff = posMod(long(rotation * stepsPerRotation_) - currentRotation(), stepsPerRotation_);
        Serial.print("spin ");
        Serial.print(diff);
        Serial.print("  ");
        Serial.println(diff + stepsPerRotation_ * extraRounds);
        stepper_->move(diff + stepsPerRotation_ * extraRounds);
    }
}

void Stepper::stop()
{
    if (stepper_) {
        stepper_->stopMove();
    }
}

bool Stepper::running()
{
    if (stepper_) {
        return stepper_->isRunning();
    }
    return false;
}

long Stepper::currentPosition()
{
    if (stepper_) {
        return stepper_->getCurrentPosition();
    }
    return 0;
}
int Stepper::currentRotation()
{
    return posMod(currentPosition(), stepsPerRotation_);
}

StepperGroup::StepperGroup(Stepper *steppers[], int n):
    steppers_(steppers), n_(n)
{
}

void StepperGroup::allDirectTo(float rotation)
{
    for (int i = 0; i < n_; i++) {
        steppers_[i]->directTo(rotation);
    }
}
void StepperGroup::stopAll()
{
    for (int i = 0; i < n_; i++) {
        steppers_[i]->stop();
    }
}
bool StepperGroup::anyRunning() const
{
    for (int i = 0; i < n_; i++) {
        if (steppers_[i]->running()) {
            return true;
        }
    }
    return false;
}
Stepper* StepperGroup::get(int i) const
{
    return i < n_ ? steppers_[i] : nullptr;
}