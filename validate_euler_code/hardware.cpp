#include "hardware.h"

#include <Arduino.h>

Button::Button(uint8_t pin, uint8_t mode, uint8_t activeValue):
    prevState_(false),
    currState_(false),
    pin_(pin),
    mode_(mode),
    activeValue_(activeValue),
    since_(millis())
{
}
Button::Button(uint8_t pin): Button(pin, INPUT_PULLUP, LOW)
{
}

void Button::begin()
{
    pinMode(pin_, mode_);
    since_ = millis();
    update();
}

void Button::update()
{
    prevState_ = currState_;
    currState_ = digitalRead(pin_) == activeValue_;
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

long Button::lastToggled() const
{
    return since_;
}

byte rightArrow[] = {
  B00000,
  B01000,
  B01100,
  B01110,
  B01111,
  B01110,
  B01100,
  B01000
};

LCD::LCD(uint8_t addr): lcd_(addr, 20, 4)
{
}

void LCD::begin()
{
    lcd_.init();
    lcd_.createChar(LCD_CHAR_ARROW, rightArrow);
    lcd_.backlight();
}

void LCD::write(uint8_t row, uint8_t col, uint8_t ch)
{
    lcd_.setCursor(col, row);
    lcd_.write(ch);
}
void LCD::print(uint8_t row, uint8_t col, char ch)
{
    lcd_.setCursor(col, row);
    lcd_.print(ch);
}
void LCD::print(uint8_t row, uint8_t col, uint8_t width, const char* str)
{
    lcd_.setCursor(col, row);
    char buffer[width + 1];
    buffer[width] = '\0';
    bool terminated = false;
    for (int i = 0; i < width; i++) {
        if (!terminated) {
            char c = str[i];
            if (c == '\0') {
                terminated = true;
            } else {
                buffer[i] = c;
            }
        }
        if (terminated) {
            buffer[i] = ' ';
        }
    }
    lcd_.print(buffer);
}
void LCD::clear()
{
    lcd_.clear();
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
        return NULL;
    }
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

bool ButtonGroup::allActive() const
{
    for (int i = 0; i < n_; i++) {
        if (!buttons_[i]->isActive()) {
            return false;
        }
    }
    return true;
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
