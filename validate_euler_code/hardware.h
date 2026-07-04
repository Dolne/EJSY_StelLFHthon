#ifndef hardware_h
#define hardware_h

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

#include "task.h"

inline const int STEPPER_STEPS = 1600;

class Button: public Task
{
public:
    Button(uint8_t pin);
    Button(uint8_t pin, uint8_t mode, uint8_t activeValue);
    void begin();
    void update();
    bool toggled() const;
    bool toggled(bool active) const;
    bool isActive() const;
    long lastToggled() const;
private:
    uint8_t pin_;
    uint8_t mode_;
    bool prevState_;
    bool currState_;
    int activeValue_;
    long since_;
};

class ButtonGroup: public Task
{
public:
    ButtonGroup(Button* buttons[], int n);
    void begin();
    void update();
    Button* get(int i) const;
    bool anyToggled(bool active) const;
    bool anyActive() const;
    bool allActive() const;
    long lastToggled() const;
private:
    Button** buttons_;
    int n_;
};

inline const uint8_t LCD_CHAR_ARROW = 0;

class LCD
{
public:
    LCD(uint8_t addr);
    void begin();
    void write(uint8_t row, uint8_t col, uint8_t ch);
    void print(uint8_t row, uint8_t col, char ch);
    void print(uint8_t row, uint8_t col, uint8_t width, const char* str);
    void clear();
private:
    LiquidCrystal_I2C lcd_;
};

#endif