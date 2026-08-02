#ifndef game_h
#define game_h

#include <Arduino.h>
#include <AccelStepper.h>
#include <FastLED.h>
#include "hardware.h"
#include "button.h"
#include "state.h"

// option names to show in menu

inline const char *OPTS_ON_OFF[] = {"off", "on"};
// this is meant to accomodate up to the stimuli with the most features
inline const char *OPTS_DIFFS[] = {"off", "manual", "1 diff", "2 diff", "3 diff", "4 diff"};

inline const char *OPTS_VOLUME[] = {"low", "medium", "high"};
inline const uint8_t VOLUME_VALUES[] = {10, 20, 30};

inline const char *OPTS_SCAN_SPEED[] = {"2s", "5s", "10s"};
inline const int SCAN_SPEED_VALUES[] = {2000, 5000, 10000};

inline const char *OPTS_SLOTS[] = {"3", "4"};
inline const int MIN_SLOTS = 3;

inline const char *OPTS_ROUNDS[] = {"1", "2", "3", "4"};
inline const int MIN_ROUNDS = 1;

inline const char *OPTS_INPUT_MODE[] = {"scanning", "select"};
inline const int INPUT_MODE_SCANNING = 0;
inline const int INPUT_MODE_SELECT = 1;

inline const int VISUAL_FEATS_COUNT = 3; // size colour shape
inline const int VISUAL_VALUE_OFFSET = 2; // null and display/reserved
inline const int VISUAL_TOTAL_VALUES = (1 << VISUAL_FEATS_COUNT) + VISUAL_VALUE_OFFSET;
inline const int AUDIO_FEATS_COUNT = 4; // loudness pitch timbre L/R
inline const int AUDIO_VALUE_OFFSET = 1; // null

// option labels to show in menu
inline const char *VISUAL_FEATS_NAMES[VISUAL_FEATS_COUNT] = { "  size", "  colour", "  shape" };
inline const char *AUDIO_FEATS_NAMES[AUDIO_FEATS_COUNT] = { "  loudness", "  pitch", "  timbre", "  L/R" };

inline const int MAX_SLOTS = 4;

class GameHardware
{
public:
    GameHardware(LCD &lcd, const ButtonGroup &inputButtons, const Button &debugButton, StepperGroup &steppers, AudioPlayer &audio, CRGB *scanningLeds, int scanningLedCount, CRGB *feedbackLeds, int feedbackLedCount, OutputController &vibration, const int *v1, int l1, const int *v2, int l2);
    LCD &lcd;
    const ButtonGroup &inputButtons;
    const Button &debugButton;
    StepperGroup &steppers;
    AudioPlayer &audio;
    CRGB *scanningLeds;
    int scanningLedCount;
    CRGB *feedbackLeds;
    int feedbackLedCount;
    OutputController &vibration;
    const int *vibrationSeqSuccess;
    int vibrationSeqSuccessLen;
    const int *vibrationSeqFail;
    int vibrationSeqFailLen;
};


class GameOptions
{
public:
    bool enabled = false;

    uint8_t rounds = 0;
    uint8_t inputMode = 0;
    uint8_t scanSpeed = 0;
    uint8_t slotsCount = 0;
    uint8_t retries = 1;
    uint8_t volume = 2;

    uint8_t visual = 0;
    uint8_t visualOptions[VISUAL_FEATS_COUNT] = {0};
    uint8_t audio = 0;
    uint8_t audioOptions[AUDIO_FEATS_COUNT] = {0};
    uint8_t tactile = 0;
};

class GameRound
{
public:
    /**
     * constructor which receives the game options to generate a game round
     */
    GameRound(GameOptions opts);
    /**
     * the number of slots used for the round. should be 3 or 4.
     */
    uint8_t slotsCount;
    /**
     * the input mode configured. either 0 (`INPUT_MODE_SCANNING`) or 1 (`INPUT_MODE_SELECT`).
     */
    uint8_t inputMode;
    /**
     * the actual/correct odd one slot. 0 <= odd1OutSlot < slotsCount.
     */
    uint8_t odd1OutSlot;
    /**
     * the user's answer. 0xFF if no answer given.
     */
    uint8_t answer = 0xFF;

    /**
     * whether there is any visual stimuli for the round
     * if `false`, `visual` will be all 0
     */
    bool hasVisual = false;
    /**
     * the index of the option to show for each slot
     * any unused slots will have a value of 0
     */
    uint8_t visual[MAX_SLOTS] = {0};

    /**
     * whether there is any audio stimuli for the round
     * if `false`, `audio` will be all 0
     */
    bool hasAudio = false;
    /**
     * the index of the option to play for each slot
     * any unused slot will have a value of 0
     */
    uint8_t audio[MAX_SLOTS] = {0};

    /**
     * whether there is any tactile stimuli for the round
     */
    bool hasTactile = false;
    /**
     * whether this is a null round, meaning there is no stimuli active in the round
     */
    bool isNullRound = false;
};

/**
 * print out the details of a game round for debugging
 */
void printGameRound(GameRound *gameRound);

#endif
