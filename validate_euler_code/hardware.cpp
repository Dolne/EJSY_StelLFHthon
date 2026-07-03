#include "hardware.h"

Button::Button(uint8_t pin, uint8_t mode)
{
    prevState_ = false;
    currState_ = false;
    pin_ = pin;
    mode_ = mode;
    activeValue_ = (mode == INPUT_PULLUP) ? LOW : HIGH;
}
Button::Button(uint8_t pin): Button(pin, INPUT_PULLUP)
{
}

void Button::begin()
{
    pinMode(pin_, mode_);
    update();
}

void Button::update()
{
    prevState_ = currState_;
    currState_ = digitalRead(pin_) == activeValue_;
}

bool Button::toggled(bool active) const
{
    return prevState_ != currState_ && currState_ == active;
}

bool Button::isActive() const
{
    return currState_;
}


LCD::LCD(uint8_t addr): lcd_(addr, 20, 4)
{
    
}

void LCD::begin()
{
    lcd_.init();
    lcd_.backlight();
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