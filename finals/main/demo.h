#ifndef demo_h
#define demo_h

#include <Arduino.h>
#include "task.h"
#include "game.h"
#include "runner.h"

class DemoRunner : public Task
{
public:
    DemoRunner(GameHardware &hardware, const GameOptions &gameOpts);
    char currName[21] = "";
    char odd1Out[21] = "odd one out: ";
    bool currNameChanged();
    bool odd1OutChanged();
    void start();
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
    bool optionsChanged_ = false;
    bool odd1OutChanged_ = false;
    const GameOptions &gameOpts_;
};

#endif