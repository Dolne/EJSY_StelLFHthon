#ifndef hardware_h
#define hardware_h

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <AccelStepper.h>
#include <Adafruit_MCP23XXX.h>
#include <MD_YX5300.h>

#include "task.h"

inline const int STEPPER_STEPS = 1600;

class Pin
{
public:
    /** 
     * represents a GPIO on the ESP32.
     * this is a converting constructor.
     */
    Pin(uint8_t pin);
    /**
     * 
     */
    Pin(uint8_t pin, Adafruit_MCP23XXX* expander);
    void pinMode(uint8_t mode);
    uint8_t digitalRead();
    void digitalWrite(uint8_t val);

private:
    uint8_t pin_;
    Adafruit_MCP23XXX* expander_ = nullptr;
};

class Button: public Task
{
public:
    Button(Pin pin);
    Button(Pin pin, uint8_t mode, uint8_t activeValue);
    void begin();
    void update();
    bool toggled() const;
    bool toggled(bool active) const;
    bool isActive() const;
    long lastToggled() const;
private:
    Pin pin_;
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

class Stepper: public Task
{
public:
    Stepper(uint8_t stepPin, uint8_t dirPin, int stepsPerRotation, float maxSpeed, float maxAcceleration);
    void begin();
    void update();
    void directTo(float rotation);
    void spinTo(float rotation, int extraRounds);
    void stop();
    bool running();
private:
    long currentPosition();
    int currentRotation();
    AccelStepper stepper_;
    int stepsPerRotation_;
    float maxSpeed_;
    float acceleration_;
};

class StepperGroup: public Task
{
public:
    StepperGroup(Stepper* steppers[], int n);
    void begin();
    void update();
    void stopAll();
    bool anyRunning() const;
    Stepper* get(int i) const;
private:
    Stepper** steppers_;
    int n_;
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
    OutputController(Pin pin);
    void begin();
    void update();
    void enable();
    void startSequence(const int seq[], int seqLen);
    void disable();
private:
    Pin pin_;
    const int *seq_ = nullptr;
    int seqLen_ = 0;
    int seqDuration_ = 0;
    long seqStart_ = 0;
};

#endif