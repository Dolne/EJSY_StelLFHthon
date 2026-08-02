#ifndef hardware_h
#define hardware_h

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_MCP23XXX.h>
#include <MD_YX5300.h>

#include "task.h"

inline const int STEPPER_STEPS = 1600;

class HardwarePin
{
public:
    /** 
     * represents a GPIO on the ESP32.
     * this is a converting constructor.
     */
    HardwarePin(uint8_t pin);
    /**
     * 
     */
    HardwarePin(uint8_t pin, Adafruit_MCP23XXX* expander);
    void pinMode(uint8_t mode);
    uint8_t digitalRead();
    void digitalWrite(uint8_t val);

private:
    uint8_t pin_;
    Adafruit_MCP23XXX* expander_ = nullptr;
};

inline const uint8_t LCD_CHAR_ARROW = 1;
inline const uint8_t LCD_CHAR_BUTTON_DOWN = 2;
inline const uint8_t LCD_CHAR_BUTTON_UP = 3;

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

class AudioPlayer: public Task
{
public:
    AudioPlayer(HardwareSerial& serial, uint8_t rxPin, uint8_t txPin);
    void begin();
    void update();
    void play(uint8_t track);
    void play(uint8_t folder, uint8_t track);
    void stop();
    void setVolume(uint8_t volume);
    bool playing();

private:
    uint8_t rxPin_;
    uint8_t txPin_;
    HardwareSerial& serial_;
    MD_YX5300 mp3_;
    bool playing_ = false;

    void handleStatus_(const MD_YX5300::cbData *data);
};

class OutputController: public Task
{
public:
    OutputController(HardwarePin pin);
    void begin();
    void update();
    void enable();
    void startSequence(const int seq[], int seqLen);
    void disable();
private:
    HardwarePin pin_;
    const int *seq_ = nullptr;
    int seqLen_ = 0;
    int seqDuration_ = 0;
    long seqStart_ = 0;
};

#endif