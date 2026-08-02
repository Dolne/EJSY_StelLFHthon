#ifndef demo_h
#define demo_h

#include <Arduino.h>
#include "task.h"
#include "game.h"
#include "runner.h"

class DemoRunner : public Task
{
public:
    DemoRunner(GameHardware &hardware);
    char currName[21] = "";
    bool currNameChanged();
    void start(uint8_t volume);
    void end();
    void reshuffle();
    void next();
    bool isActive();
    void begin();
    void update();

private:
    GameHardware &hardware_;
    GameRound *round_;
    ScanningRunner runner_;
    uint8_t currStimuli_ = 0;
    uint8_t currFeat_ = 0;
    bool stateChanged_ = false;
    bool nameChanged_ = false;
    uint8_t volume_ = 0;
};

#endif