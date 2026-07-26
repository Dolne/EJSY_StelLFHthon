#ifndef game_h
#define game_h

#include <Arduino.h>
#include <AccelStepper.h>
#include <Adafruit_NeoPixel.h>
#include "hardware.h"
#include "button.h"
#include "state.h"

inline const char* OPTS_ON_OFF[] = { "off", "on" };
// this is meant to accomodate up to the stimuli with the most features
// TODO [ux] does it make sense for manual to come first? 
inline const char* OPTS_DIFFS[] = { "off", "manual", "1 diff", "2 diff", "3 diff", "4 diff" };
inline const char* OPTS_VOLUME[] = { "low", "medium", "high" };
inline const uint8_t VOLUME_VALUES[] = { 10, 20, 30 };
inline const char* OPTS_SCAN_SPEED[] = { "2s", "5s", "10s" };
inline const int SCAN_SPEED_VALUES[] = { 2000, 5000, 10000 };

inline const char* OPTS_SLOTS[] = { "3", "4" };
inline const int MIN_SLOTS = 3;

inline const char* OPTS_ROUNDS[] = { "1", "2", "3", "4" };
inline const int MIN_ROUNDS = 1;

inline const char* OPTS_INPUT_MODE[] = { "scanning", "select" };
inline const int INPUT_MODE_SCANNING = 0;
inline const int INPUT_MODE_SELECT = 1;

inline const int VISUAL_FEATS_COUNT = 3;  // shape colour size
inline const int VISUAL_VALUE_OFFSET = 2;
inline const int VISUAL_TOTAL_VALUES = (1 << VISUAL_FEATS_COUNT) + VISUAL_VALUE_OFFSET;
inline const int AUDIO_FEATS_COUNT = 4;   // loudness pitch timbre L/R
inline const int AUDIO_VALUE_OFFSET = 1;

inline const int MAX_SLOTS = 4;

class GameHardware
{
public:
    GameHardware(LCD& lcd, const ButtonGroup& inputButtons, const Button& debugButton, StepperGroup& steppers, AudioPlayer& audio, Adafruit_NeoPixel& scanningStrip, Adafruit_NeoPixel& feedbackStrip, OutputController& vibration, const int* v1, int l1, const int* v2, int l2);
    LCD& lcd;
    const ButtonGroup& inputButtons;
    const Button& debugButton;
    StepperGroup& steppers;
    AudioPlayer& audio;
    Adafruit_NeoPixel& scanningStrip;
    Adafruit_NeoPixel& feedbackStrip;
    OutputController& vibration;
    const int* vibrationSeqSuccess;
    int vibrationSeqSuccessLen;
    const int* vibrationSeqFail;
    int vibrationSeqFailLen;
};

// for custom game mode create another options class and add a constructor to GameRound

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
    uint8_t visualOptions[VISUAL_FEATS_COUNT] = { 0 };
    uint8_t audio = 0;
    uint8_t audioOptions[AUDIO_FEATS_COUNT] = { 0 };
    uint8_t tactile = 0;
};

class GameRound
{
public:
    GameRound(GameOptions opts);
    uint8_t slotsCount; // 3 or 4
    uint8_t inputMode;
    /**
     * The actual/correct odd one slot. 0 <= odd1OutSlot < slotsCount.
     */
    uint8_t odd1OutSlot;
    /**
     * The user's answer. 0xFF if no answer given.
     */
    uint8_t answer = 0xFF;
    bool hasVisual = false;
    uint8_t visual[MAX_SLOTS] = { 0 };
    bool hasAudio = false;
    uint8_t audio[MAX_SLOTS] = { 0 };
    bool hasTactile = false;
    bool isNullRound = false;
};

void printGameRound(GameRound* gameRound);

#endif
