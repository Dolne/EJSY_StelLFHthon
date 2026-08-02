#include "button.h"

Button::Button(HardwarePin pin, uint8_t mode):
    prevState_(false),
    currState_(false),
    pin_(pin),
    mode_(mode),
    activeValue_(0),
    since_(millis())
{
}
Button::Button(HardwarePin pin): Button(pin, INPUT_PULLUP)
{
}

void Button::begin()
{
    pin_.pinMode(mode_);
    // assumes button is not pressed on setup
    activeValue_ = !pin_.digitalRead();
    since_ = millis();
    update();
}

void Button::update()
{
    prevState_ = currState_;
    currState_ = pin_.digitalRead() == activeValue_;
    if (toggled()) {
        since_ = millis();
    }
}

bool Button::toggled() const
{
    return prevState_ != currState_;
}
bool Button::toggled(bool active) const
{
    return toggled() && currState_ == active;
}

bool Button::isActive() const
{
    return currState_;
}
bool Button::rawValue() const
{
    if (currState_) {
        return activeValue_;
    } else {
        return !activeValue_;
    }
}

/**
 * Reset the active value of the button based on the currently expected state.
 * 
 * This must only be called once during each update cycle.
 */
void Button::resetActiveValue(bool currActive)
{
    // flip the activeValue_ if the current state does not match the expected state
    if (currState_ != currActive) {
        activeValue_ = !activeValue_;
    }
}

long Button::lastToggled() const
{
    return since_;
}

ButtonGroup::ButtonGroup(Button* buttons[], int n):
    buttons_(buttons),
    n_(n)
{
}

void ButtonGroup::begin()
{
    for (int i = 0; i < n_; i++) {
        buttons_[i]->begin();
    }
}

void ButtonGroup::update()
{
    for (int i = 0; i < n_; i++) {
        buttons_[i]->update();
    }
}

Button *ButtonGroup::get(int i) const
{
    if (i < n_) {
        return buttons_[i];
    } else {
        return nullptr;
    }
}

int ButtonGroup::size() const
{
    return n_;
}

bool ButtonGroup::anyToggled() const
{
    for (int i = 0; i < n_; i++) {
        if (buttons_[i]->toggled()) {
            return true;
        }
    }
    return false;
}

bool ButtonGroup::anyToggled(bool active) const
{
    for (int i = 0; i < n_; i++) {
        if (buttons_[i]->toggled(active)) {
            return true;
        }
    }
    return false;
}

bool ButtonGroup::anyActive() const
{
    for (int i = 0; i < n_; i++) {
        if (buttons_[i]->isActive()) {
            return true;
        }
    }
    return false;
}

void ButtonGroup::resetActiveValues(bool currActive)
{
    for (int i = 0; i < n_; i++) {
        buttons_[i]->resetActiveValue(currActive);
    }
}

long ButtonGroup::lastToggled() const
{
    long latest = 0;
    for (int i = 0; i < n_; i++) {
        long toggled = buttons_[i]->lastToggled();
        if (toggled > latest) {
            latest = toggled;
        }
    }
    return latest;
}

const uint8_t BUTTON_INFO_WIDTH = 5;

void updateButtonGroupInfo(ButtonGroup& group, char *infoStr) {
    int b = 0;
    for (int i = 0; i < 20; i++) {
        if (i == b * BUTTON_INFO_WIDTH) {
            if (group.get(b)->rawValue()) {
                infoStr[i] = '1';
            } else {
                infoStr[i] = '0';
            }
        } else if (i == b * BUTTON_INFO_WIDTH + 1) {
            if (group.get(b)->isActive()) {
                infoStr[i] = LCD_CHAR_BUTTON_DOWN;
            } else {
                infoStr[i] = LCD_CHAR_BUTTON_UP;
            }
            if (b < group.size() - 1) {
                b++;
            }
        } else {
            infoStr[i] = ' ';
        }
    }
    infoStr[20] = 0;
}