#ifndef hardware_h
#define hardware_h

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

#include "task.h"

class Button: public Task
{
public:
    Button(uint8_t pin);
    Button(uint8_t pin, uint8_t mode);
    void begin();
    void update();
    bool toggled(bool active) const;
    bool isActive() const;
private:
    uint8_t pin_;
    uint8_t mode_;
    bool prevState_;
    bool currState_;
    int activeValue_;
};

class LCD
{
public:
    LCD(uint8_t addr);
    void begin();
    void print(uint8_t row, uint8_t col, char ch);
    void print(uint8_t row, uint8_t col, uint8_t width, const char* str);
    void clear();
private:
    LiquidCrystal_I2C lcd_;
};

#endif