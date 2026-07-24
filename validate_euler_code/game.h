#ifndef game_h
#define game_h

#include <Arduino.h>
#include <AccelStepper.h>
#include <Adafruit_NeoPixel.h>
#include "hardware.h"
#include "state.h"

inline const char* OPTS_ON_OFF[] = { "off", "on" };
// this is meant to accomodate up to the stimuli with the most features
// TODO [ux] does it make sense for manual to come first? 
inline const char* OPTS_DIFFS[] = { "off", "manual", "1 diff", "2 diff", "3 diff", "4 diff" };

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
inline const int TACTILE_FEATS_COUNT = 2; // texture temperature

inline const int MAX_SLOTS = 4;

class GameHardware
{
public:
    GameHardware(LCD& lcd, const ButtonGroup& inputButtons, const ButtonGroup& configButtons, StepperGroup& steppers, AudioPlayer& audio, Adafruit_NeoPixel& scanningStrip, Adafruit_NeoPixel& feedbackStrip, OutputController& vibration, const int* v1, int l1, const int* v2, int l2);
    LCD& lcd;
    const ButtonGroup& inputButtons;
    const ButtonGroup& configButtons;
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
    uint8_t slotsCount = 0;

    uint8_t visual = 0;
    uint8_t visualOptions[VISUAL_FEATS_COUNT] = { 0 };
    uint8_t audio = 0;
    uint8_t audioOptions[AUDIO_FEATS_COUNT] = { 0 };
    uint8_t tactile = 0;
    uint8_t tactileOptions[TACTILE_FEATS_COUNT] = { 0 };
};

class GameRound
{
public:
    GameRound(GameOptions opts);
    uint8_t slotsCount; // 3 or 4
    uint8_t inputMode;
    uint8_t odd1OutSlot;
    uint8_t answer = 0xFF;
    bool hasVisual = false;
    uint8_t visual[MAX_SLOTS] = { 0 };
    bool hasAudio = false;
    uint8_t audio[MAX_SLOTS] = { 0 };
    bool hasTactile = false;
    uint8_t tactile[MAX_SLOTS] = { 0 };
};

void printGameRound(GameRound* gameRound);

#endif
