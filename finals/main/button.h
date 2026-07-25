#ifndef button_h
#define button_h

#include "task.h"
#include "hardware.h"

class Button: public Task
{
public:
    Button(Pin pin);
    Button(Pin pin, uint8_t mode);
    void begin();
    void update();
    bool toggled() const; //Both if button pressed/released
    bool toggled(bool active) const; //Only if button is pressed
    bool isActive() const; //If button is pressed (active state)
    bool rawValue() const;
    void resetActiveValue(bool active);
    long lastToggled() const; //Last time button was toggled
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
    int size() const;
    bool anyToggled() const;
    bool anyToggled(bool active) const; //If any buttons were toggled this update (pressed/released)
    bool anyActive() const; //If any buttons are currently pressed
    void resetActiveValues(bool active);
    long lastToggled() const;
private:
    Button** buttons_;
    int n_;
};

void updateButtonGroupInfo(ButtonGroup& group, char *infoStr);

#endif