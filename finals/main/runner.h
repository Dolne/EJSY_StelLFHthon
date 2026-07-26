#ifndef runner_h
#define runner_h

#ifndef SIMULATION
#define SIMULATION 0
#endif

#include <Arduino.h>
#include "task.h"
#include "game.h"
#include "state.h"

enum class ScanStage
{
    /**
     * start of slot being selected
     */
    SLOT_START,
    /**
     * playing slot number audio
     */
    INITIAL_AUDIO,
    /**
     * playing the actual audio if any
     */
    AUDIO,
    /**
     * special stage triggered by `wait` function
     */
    WAITING,
    /**
     * end of slot
     */
    SLOT_END,
    /**
     * runner is stopped
     */
    STOPPED
};

class ScanningRunner : public Task
{
public:
    ScanningRunner(const GameHardware &hardware);
    void begin();
    void update();

    /**
     * start the scanning runner for the given game round. each slot will be selected for the specified scan duration.
     */
    void startScanning(GameRound *round, int scanDuration);
    /**
     * stop the scanning runner
     */
    void stop();

    /**
     * return the current slot that the scanning runner is at.
     * returns 0xFF if no slot is currently selected.
     */
    uint8_t slot();

private:
    /**
     * wait for a specified duration before going to the specified scan stage
     */
    void wait(long duration, ScanStage next);

    /**
     * reference to the game hardware
     */
    const GameHardware &hardware_;

    /**
     * the current scan stage of the runner
     */
    State<ScanStage> scanStage_{};

    /**
     * the game round that the runner is currently scanning
     */
    GameRound *round_{};

    /**
     * the current slot number selected
     * this will already be set to the upcoming slot when waiting between slots
     */
    uint8_t slot_{};

    /**
     * the time (`millis()`) at which the current slot was started
     */
    long slotStart_{};

    /**
     * duration to scan each slot for (in milliseconds)
     */
    int scanDuration_ = 2000;

    /**
     * the duration to wait before going to the waitNext_ stage
     */
    long waitDuration_{};
    /**
     * the stage to go to after waiting for waitDuration_
     */
    ScanStage waitNext_{};
};

enum class GameStage
{
    /**
     * showing game options menu
     */
    CONFIG,
    /**
     * showing debug menu
     */
    DEBUG_MENU,
    /**
     * starts the game if there are valid options
     */
    STARTING,
    /**
     * spinning steppers (if visual stimuli enabled)
     */
    SPINNING,
    /**
     * awaiting user input. can be either scanning or select input modes.
     */
    SELECTION,
    /**
     * feedback to user after answering
     */
    FEEDBACK,
    /**
     * stop the game.
     * this stage will remain until everything (particularly the steppers) have fully stopped.
     */
    STOPPING,
    /**
     * game is stopped
     */
    STOPPED
};

class GameRunner : public Task
{
public:
    GameRunner(const GameHardware &hardware);
    void begin();
    void update();

    /**
     * start the game with the given options.
     *
     * this will first reset the currently running game if any.
     */
    void startGame(GameOptions opts);

    /**
     * end/reset the current game. this will bring the user back to the game options menu.
     */
    void reset();

    /**
     * get the current game stage.
     */
    const State<GameStage> &stage();

    /**
     * returns the total score of the current game.
     */
    uint8_t score();

    /**
     * start the next round of the game.
     * this will end the game if there is no next round.
     */
    void nextRound();
    /**
     * returns the current round number, starting from 0.
     */
    uint8_t currRound();
    /**
     * returns the total number of rounds in the current game.
     * returns 0 if there is no active game.
     */
    uint8_t totalRounds();

    /**
     * returns the correct answer for the current round.
     * returns 0xFF if there is no active round or it is in null gameplay mode.
     */
    uint8_t roundAnswer();
    /**
     *
     */
    bool hasNextRound();

    /**
     * returns true if user has already answered with a wrong answer and retries are enabled
     */
    bool canRetry();
    /**
     * retry the current round
     */
    void retry();

    /**
     * show the debug menu.
     * this will sort of pause the runner.
     */
    void enterDebug();
    /**
     * exit the debug menu and go back to the previous game stage.
     */
    void exitDebug();

private:
    void spinSteppers();

    /**
     * the options for the current game.
     * options_.enabled will be false if there is no game.
     * this is a copy of the options passed in startGame.
     */
    GameOptions options_{};

    /**
     * reference to the game hardware
     */
    const GameHardware &hardware_;

    /**
     * the current game stage of the runner
     */
    State<GameStage> gameStage_{};

    /**
     * the scanning runner used to do scanning input mode and play audio
     */
    ScanningRunner scanningRunner_;

    /**
     * the current game round. will be null if there is no active round.
     */
    GameRound *round_{};

    /**
     * the total score of the current game
     */
    uint8_t score_{};
    /**
     * the number of rounds that have been completed before the current one
     * theis will go from 0 to (number of rounds - 1)
     */
    uint8_t rounds_{};
};

#endif