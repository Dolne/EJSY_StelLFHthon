#include <Arduino.h>
#include <Wire.h>

#include <Adafruit_MCP23X17.h>
#include <FastLED.h>

#ifndef SIMULATION // SIMULATION is defined as 1 in the simulation compiler config 
#define SIMULATION 0
#endif

#include "task.h"
#include "hardware.h"
#include "button.h"
#include "menu.h"
#include "game.h"
#include "runner.h"
#include "demo.h"

// MCP23017 I2C GPIO expander
const uint8_t MCP_ADDR = 0x20;
Adafruit_MCP23X17 expander;

// YX5300 serial MP3 player on Serial2
const uint8_t MP3_RX = 16; // ESP RX2 connected to YX5300 TX
const uint8_t MP3_TX = 17; // ESP TX2 connected to YX5300 RX

// LCD HD44780 display over I2C
const int LCD_ADDR = 0x27;

// ESP32 on board boot, used as a "last resort" for debugging
const uint8_t BUTTON_BOOT_PIN = 0;

// gamemaster buttons (up, action, down)
#if SIMULATION
    // connected to esp in simulation
    const uint8_t BUTTON_UP_PIN = 35;
    const uint8_t BUTTON_ACTION_PIN = 34;
    const uint8_t BUTTON_DOWN_PIN = 39;
#else
    // connected to MCP23017 for actual
    const HardwarePin BUTTON_UP_PIN = HardwarePin(0, &expander);
    const HardwarePin BUTTON_ACTION_PIN = HardwarePin(1, &expander);
    const HardwarePin BUTTON_DOWN_PIN = HardwarePin(2, &expander);
#endif

// user control buttons/switches (via 3.5mm mono audio jack)
const uint8_t BUTTON_1_PIN = 5;
const uint8_t BUTTON_2_PIN = 18;
const uint8_t BUTTON_3_PIN = 19;
const uint8_t BUTTON_4_PIN = 23;

// relay for vibration motor
const uint8_t VIBRATION_PIN = 2; // can be a Pin with &expander also
// vibration patterns in sequence of on, off, on, off, ...
const int VIBRATION_SUCCESS[] = {500, 500, 500, 500, 500, 1500};
const int VIBRATION_SUCCESS_LEN = 6;
const int VIBRATION_FAIL[] = {2500, 500};
const int VIBRATION_FAIL_LEN = 2;

// NeoPixel led strips for scanning and feedback
const uint8_t SCANNING_LED_PIN = 4;
const uint8_t SCANNING_LED_COUNT = MAX_SLOTS;
const uint8_t FEEDBACK_LED_PIN = 25;
const uint8_t FEEDBACK_LED_COUNT = 30;

// 200 steps per rotation x 8 microsteps = 1600
const int STEPS_PER_ROTATION = 1600;
// stepper max speed and acceleration to limit current draw
const int STEPPER_MAX_SPEED = 3000;
const int STEPPER_ACCELERATION = 800;

// step and dir pins connected to TMC2160-OC stepper driver
const uint8_t STEPPER_1_STEP = 26;
const uint8_t STEPPER_1_DIR = 12;
const uint8_t STEPPER_2_STEP = 27;
const uint8_t STEPPER_2_DIR = 13;
const uint8_t STEPPER_3_STEP = 32;
const uint8_t STEPPER_3_DIR = 14;
const uint8_t STEPPER_4_STEP = 33;
const uint8_t STEPPER_4_DIR = 15;

// currently enable pin is not used and lightgates for homing not implemented

// how many times a second to check for updates from inputs to update game stage
const int TICKRATE = 100;

Button buttonBoot(BUTTON_BOOT_PIN);

// active values of buttons are auto detected on setup by assuming they are not pressed
#if SIMULATION
    // simulated gamemaster buttons are normally open and connected with an external pull-down resistor
    Button buttonUp(BUTTON_UP_PIN, INPUT);
    Button buttonAction(BUTTON_ACTION_PIN, INPUT);
    Button buttonDown(BUTTON_DOWN_PIN, INPUT);
#else
    // actual gamemaster buttons are normally closed
    // so with INPUT_PULLUP they will HIGH when pressed
    Button buttonUp(BUTTON_UP_PIN);
    Button buttonAction(BUTTON_ACTION_PIN);
    Button buttonDown(BUTTON_DOWN_PIN);
#endif

Button* configButtonList[] = { &buttonUp, &buttonAction, &buttonDown };
ButtonGroup configButtons(configButtonList, 3);

Button button1(BUTTON_1_PIN);
Button button2(BUTTON_2_PIN);
Button button3(BUTTON_3_PIN);
Button button4(BUTTON_4_PIN);
Button* inputButtonList[] = { &button1, &button2, &button3, &button4 };
ButtonGroup inputButtons(inputButtonList, MAX_SLOTS);

CRGB scanningLeds[SCANNING_LED_COUNT];
CRGB feedbackLeds[FEEDBACK_LED_COUNT];

void initLeds() {
    FastLED.addLeds<WS2812B, SCANNING_LED_PIN, GRB>(scanningLeds, SCANNING_LED_COUNT);
    FastLED.addLeds<WS2812B, FEEDBACK_LED_PIN, GRB>(feedbackLeds, FEEDBACK_LED_COUNT);
}

OutputController vibration(VIBRATION_PIN);

Stepper stepper1(STEPPER_1_STEP, STEPPER_1_DIR, STEPS_PER_ROTATION, STEPPER_MAX_SPEED, STEPPER_ACCELERATION);
Stepper stepper2(STEPPER_2_STEP, STEPPER_2_DIR, STEPS_PER_ROTATION, STEPPER_MAX_SPEED, STEPPER_ACCELERATION);
Stepper stepper3(STEPPER_3_STEP, STEPPER_3_DIR, STEPS_PER_ROTATION, STEPPER_MAX_SPEED, STEPPER_ACCELERATION);
Stepper stepper4(STEPPER_4_STEP, STEPPER_4_DIR, STEPS_PER_ROTATION, STEPPER_MAX_SPEED, STEPPER_ACCELERATION);
Stepper* stepperList[MAX_SLOTS] = { &stepper1, &stepper2, &stepper3, &stepper4 };

StepperGroup steppers(stepperList, MAX_SLOTS);

AudioPlayer audio(Serial2, MP3_RX, MP3_TX);

LCD lcd(LCD_ADDR);

GameOptions options{};

// wrapper to pass all the hardware to game runner
GameHardware gameHardware(lcd, inputButtons, buttonBoot, steppers, audio, scanningLeds, SCANNING_LED_COUNT, feedbackLeds, FEEDBACK_LED_COUNT, vibration, VIBRATION_SUCCESS, VIBRATION_SUCCESS_LEN, VIBRATION_FAIL, VIBRATION_FAIL_LEN);

GameRunner runner(gameHardware);

DemoRunner demoRunner(gameHardware, options);

uint8_t demoModeEnabled = 0;

// tasks to run at TICKRATE
// the order of tasks should be inputs -> runner -> outputs
Task* taskList[] = { &configButtons, &inputButtons, &buttonBoot, &runner, &demoRunner, &vibration };
TaskGroup tasks(taskList, 5);

// gamemaster hardware
MenuHardware menuHardware(lcd, buttonUp, buttonAction, buttonDown);

void startGame() {
    runner.startGame(options);
}
void startDemo() {
    if (runner.stage().is(GameStage::CONFIG)) {
        demoRunner.start();
    }
}

// callbacks to control whether the submenu for "manual" diffs for each stimuli is shown
// this returns true when "manual" is selected for each stimuli
bool visualSubMenuHidden() {
    return options.visual != 1;
}
bool audioMenuHidden() {
    return options.inputMode != 0;
}
bool audioSubMenuHidden() {
    return audioMenuHidden() || options.audio != 1;
}

// menu for changing game options and starting the game
// the game options does not actually know/care what each option within a stimuli is
// so that means it is only defined by the order in the submenu
MenuRow* configRows[] = {
    new MenuOptionRow(menuHardware, &options.rounds, "rounds", OPTS_NUMBER, 4),
    new MenuOptionRow(menuHardware, &options.slotsCount, "slots", OPTS_SLOTS, 2),

    new MenuOptionRow(menuHardware, &options.inputMode, "input mode", OPTS_INPUT_MODE, 2),
    // scanning speed, only shown when input mode is "scanning"
    new MenuOptionRow(menuHardware, &options.scanSpeed, "  duration", OPTS_SCAN_SPEED, 3, [] () { return options.inputMode != 0; }),

    new MenuOptionRow(menuHardware, &options.retries, "retries", OPTS_ON_OFF, 2),
    new MenuOptionRow(menuHardware, &options.volume, "volume", OPTS_VOLUME, 3),

    new MenuOptionRow(menuHardware, &options.visual, "visual", OPTS_DIFFS, VISUAL_FEATS_COUNT + 2),
    new MenuOptionRow(menuHardware, &options.visualOptions[0], VISUAL_FEATS_NAMES[0], OPTS_ON_OFF, 2, visualSubMenuHidden),
    new MenuOptionRow(menuHardware, &options.visualOptions[1], VISUAL_FEATS_NAMES[1], OPTS_ON_OFF, 2, visualSubMenuHidden),
    new MenuOptionRow(menuHardware, &options.visualOptions[2], VISUAL_FEATS_NAMES[2], OPTS_ON_OFF, 2, visualSubMenuHidden),

    new MenuOptionRow(menuHardware, &options.audio, "audio", OPTS_DIFFS, AUDIO_FEATS_COUNT + 2, audioMenuHidden),
    new MenuOptionRow(menuHardware, &options.audioOptions[0], AUDIO_FEATS_NAMES[0], OPTS_ON_OFF, 2, audioSubMenuHidden),
    new MenuOptionRow(menuHardware, &options.audioOptions[1], AUDIO_FEATS_NAMES[1], OPTS_ON_OFF, 2, audioSubMenuHidden),
    new MenuOptionRow(menuHardware, &options.audioOptions[2], AUDIO_FEATS_NAMES[2], OPTS_ON_OFF, 2, audioSubMenuHidden),
    new MenuOptionRow(menuHardware, &options.audioOptions[3], AUDIO_FEATS_NAMES[3], OPTS_ON_OFF, 2, audioSubMenuHidden), // or "panning"
    new MenuOptionRow(menuHardware, &options.audioFolder, "  folder", OPTS_NUMBER, 8, [] () { return audioMenuHidden() || options.audio == 0; }),

    // tactile options only shown when input mode is "select"
    new MenuOptionRow(menuHardware, &options.tactile, "tactile", OPTS_ON_OFF, 2, [] () { return options.inputMode != 1; }),
    
    new MenuActionRow(menuHardware, "Start game", startGame),

    new MenuActionRow(menuHardware, "Enter demo mode", startDemo, [] () { return !demoModeEnabled; }),
};
Menu configMenu(menuHardware, configRows, sizeof(configRows) / sizeof(configRows[0]));


char roundInfo[21] = {};
char answerInfo[21] = {};

// menu shown while game is in progress with an option to reset/restart the game
MenuRow* roundRows[] = {
    new MenuInfoRow(menuHardware, roundInfo),
    new MenuInfoRow(menuHardware, answerInfo),
    new MenuActionRow(menuHardware, "Reset/Restart Game", [] () { runner.reset(); }),
};
Menu roundMenu(menuHardware, roundRows, sizeof(roundRows) / sizeof(roundRows[0]));

MenuActionRow retryRow(menuHardware, "Retry round", [] () { runner.retry(); }, [] () { return !runner.canRetry(); });

char scoreInfo[21] = {};

// menu shown at the end of each round with the current round number and score
MenuRow* feedbackRows[] = {
    new MenuInfoRow(menuHardware, roundInfo),
    new MenuInfoRow(menuHardware, scoreInfo),
    new MenuActionRow(menuHardware, "Next round", [] () { runner.nextRound(); }, [] () { return !runner.hasNextRound(); }),
    new MenuActionRow(menuHardware, "New game", [] () { runner.reset(); }, [] () { return runner.hasNextRound(); }),
    new MenuActionRow(menuHardware, "Retry round", [] () { runner.retry(); }, [] () { return !runner.canRetry(); }),
};
Menu feedbackMenu(menuHardware, feedbackRows, sizeof(feedbackRows) / sizeof(feedbackRows[0]), 4);

MenuRow* demoRows[] = {
    new MenuInfoRow(menuHardware, demoRunner.currName, []() { return demoRunner.currNameChanged(); }),
    new MenuActionRow(menuHardware, "Next", [] () { demoRunner.next(); }),
    new MenuActionRow(menuHardware, "Reshuffle", [] () { demoRunner.reshuffle(); }),
    new MenuActionRow(menuHardware, "Exit", [] () { demoRunner.end(); }),
};
Menu demoMenu(menuHardware, demoRows, sizeof(demoRows) / sizeof(demoRows[0]), 1);

char configButtonsInfo[21] = "";
char inputButtonsInfo[21] = "";

bool configButtonsUpdated() {
    return configButtons.anyToggled();
}
bool inputButtonsUpdated() {
    return inputButtons.anyToggled();
}

MenuRow *debugRows[] = {
    new MenuActionRow(menuHardware, "Go back", []() { return runner.exitDebug(); }),
    new MenuInfoRow(menuHardware, "Gamemaster buttons:"),
    new MenuInfoRow(menuHardware, configButtonsInfo, configButtonsUpdated),
    new MenuInfoRow(menuHardware, "User buttons:"),
    new MenuInfoRow(menuHardware, inputButtonsInfo, inputButtonsUpdated),
    new MenuActionRow(menuHardware, "Reset user buttons", [] () { inputButtons.resetActiveValues(false); }),
    new MenuOptionRow(menuHardware, &demoModeEnabled, "Demo mode", OPTS_ON_OFF, 2),
};
Menu debugMenu(menuHardware, debugRows, sizeof(debugRows) / sizeof(debugRows[0]), 0);

// menu controller to ensure only 1 menu is trying to display itself on the LCD
MenuController menus(lcd);

bool timeForNextUpdate() {
    static const int interval = 1000 / TICKRATE;
    static long nextTime = millis();
    long time = millis();
    if (time >= nextTime) {
        nextTime += ((time - nextTime) / interval + 1) * interval;
        return true;
    }
    return false;
}

void setup() {
    Serial.begin(9600);
    lcd.begin();
    lcd.print(1, 8, 4, "EJSY");
    expander.begin_I2C(MCP_ADDR);

    initLeds();

    // the buttons will use the pin value at setup as the inactive value
    // as such all buttons must not be pressed on setup
    tasks.begin();
    steppers.begin();
    audio.begin();
    // set volume to the default option value
    audio.setVolume(VOLUME_VALUES[options.volume]);

}
void loop() {
    static bool selected = false;
    static uint8_t prev = 0;
    static uint8_t prev2 = 0;
    if (timeForNextUpdate()) {
        // run things at TICKRATE
        tasks.update();

        // set the setting displayed menu using the menu controller based on game stage
        if (runner.stage().is(GameStage::DEBUG_MENU)) {
            if (configButtonsUpdated() || runner.stage().changed()) {
                updateButtonGroupInfo(configButtons, configButtonsInfo);
            }
            if (inputButtonsUpdated() || runner.stage().changed()) {
                updateButtonGroupInfo(inputButtons, inputButtonsInfo);
            }
            menus.use(&debugMenu);
        } else if (runner.stage().is(GameStage::CONFIG)) {
            if (demoRunner.isActive()) {
                menus.use(&demoMenu);
            } else {
                menus.use(&configMenu);
            }
        } else if (runner.stage().is(GameStage::SPINNING) || runner.stage().is(GameStage::SELECTION)) {
            if (runner.stage().changed()) {
                // update round number and answer strings BEFORE menu is shown
                sprintf(roundInfo, "Round %d/%d", runner.currRound() + 1, runner.totalRounds());
                if (runner.roundAnswer() < 0xFF) {
                    sprintf(answerInfo, "Answer: %d", runner.roundAnswer() + 1);
                } else {
                    sprintf(answerInfo, "Answer: NULL");
                }
            }
            menus.use(&roundMenu);
        } else if (runner.stage().is(GameStage::FEEDBACK)) {
            if (runner.stage().changed()) {
                // update score strings BEFORE menu is shown
                sprintf(scoreInfo, "Total score: %d", runner.score());
            }
            
            menus.use(&feedbackMenu);
        } else {
            // blank screen
            menus.use(nullptr);
        }

        FastLED.show();
    }

    // steppers must be able to step in every loop
    steppers.update();

    // check for response from mp3 module as frequently as possible
    audio.update();
}
