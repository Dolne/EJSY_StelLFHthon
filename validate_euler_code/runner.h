#ifndef runner_h
#define runner_h

#include <Arduino.h>
#include "task.h"
#include "game.h"
#include "state.h"

enum class ScanStage {
    SLOT_START,
    INITIAL_AUDIO,
    AUDIO,
    WAITING,
    SLOT_END,
    STOPPED
};

class ScanningRunner: public Task
{
public:
    ScanningRunner(const GameHardware& hardware);
    void startScanning(GameRound* round);
    void stop();
    void begin();
    void update();
    uint8_t slot();

private:
    void wait(long duration, ScanStage next);
    const GameHardware& hardware_;
    State<ScanStage> scanStage_{};
    GameRound* round_{};
    uint8_t slot_{};
    long waitDuration_{};
    ScanStage waitNext_{};
};

enum class GameStage {
    CONFIG,
    STARTING,
    SPINNING,
    SELECTION,
    FEEDBACK,
    STOPPING,
    STOPPED
};

class GameRunner: public Task
{
public:
    GameRunner(const GameHardware& hardware);
    /**
     * Start the game with the given options.
     * 
     * If the game is already running, it will call stopGame first.
     */
    void startGame(GameOptions opts);
    void nextRound();
    void reset();
    const State<GameStage>& stage();
    void begin();
    void update();
    uint8_t score();
    uint8_t currRound();
    uint8_t totalRounds();
    bool hasNextRound();
private:
    void spinSteppers();

    GameOptions options_{};
    const GameHardware& hardware_;
    State<GameStage> gameStage_{};
    ScanningRunner scanningRunner_;
    GameRound* round_{};
    uint8_t score_{};
    uint8_t rounds_{};
};

#endif