#include <Arduino.h>
#include <Wire.h>

#include <Adafruit_MCP23X17.h>
#include <Adafruit_NeoPixel.h>

#ifndef SIMULATION // SIMULATION is defined as 1 in the simulation compiler config 
#define SIMULATION 0
#endif

#include "hardware.h"
#include "task.h"
#include "menu.h"
#include "game.h"
#include "runner.h"

const uint8_t MCP_ADDR = 0x20;
Adafruit_MCP23X17 expander;

// GPIO 16 (RX2) and 17 (TX2) used for audio
const uint8_t MP3_RX = 16; // ESP RX2 connected to YX5300 TX
const uint8_t MP3_TX = 17; // ESP TX2 connected to YX5300 RX
const uint8_t AUDIO_VOLUME = 15;

// GPIO 21 (I2C SDA) and 22 (I2C SCL) used for I2C
const int LCD_ADDR = 0x27;

#if SIMULATION
    const uint8_t BUTTON_UP_PIN = 0;
    const uint8_t BUTTON_SELECT_PIN = 35;
    const uint8_t BUTTON_DOWN_PIN = 34;
#else
    const Pin BUTTON_UP_PIN = Pin(0, &expander);
    const Pin BUTTON_SELECT_PIN = Pin(1, &expander);
    const Pin BUTTON_DOWN_PIN = Pin(2, &expander);
#endif

const uint8_t BUTTON_1_PIN = 5;
const uint8_t BUTTON_2_PIN = 18;
const uint8_t BUTTON_3_PIN = 19;
const uint8_t BUTTON_4_PIN = 23;

const uint8_t VIBRATION_PIN = 2; // can be a Pin with &expander also
const int VIBRATION_SUCCESS[] = {500, 500, 500, 500, 500, 1500};
const int VIBRATION_SUCCESS_LEN = 6;
const int VIBRATION_FAIL[] = {2500, 500};
const int VIBRATION_FAIL_LEN = 2;

const uint8_t SCANNING_LED_PIN = 4;
const uint8_t FEEDBACK_LED_PIN = 25;
const uint8_t FEEDBACK_LED_COUNT = 32; // TODO update this for actual

const int STEPS_PER_ROTATION = 1600;
const int STEPPER_MAX_SPEED = 3000;
const int STEPPER_ACCELERATION = 800;

const uint8_t STEPPER_1_STEP = 26;
const uint8_t STEPPER_1_DIR = 12;
const uint8_t STEPPER_2_STEP = 27;
const uint8_t STEPPER_2_DIR = 13;
const uint8_t STEPPER_3_STEP = 32;
const uint8_t STEPPER_3_DIR = 14;
const uint8_t STEPPER_4_STEP = 33;
const uint8_t STEPPER_4_DIR = 15;

const int TICKRATE = 100;

#if SIMULATION
    Button buttonUp(BUTTON_UP_PIN);
    Button buttonSelect(BUTTON_SELECT_PIN);
    Button buttonDown(BUTTON_DOWN_PIN);
#else
    Button buttonUp(BUTTON_UP_PIN, INPUT_PULLUP, HIGH);
    Button buttonSelect(BUTTON_SELECT_PIN, INPUT_PULLUP, HIGH);
    Button buttonDown(BUTTON_DOWN_PIN, INPUT_PULLUP, HIGH);
#endif

Button* configButtonList[] = { &buttonUp, &buttonSelect, &buttonDown };
ButtonGroup configButtons(configButtonList, 3);

Button button1(BUTTON_1_PIN);
Button button2(BUTTON_2_PIN);
Button button3(BUTTON_3_PIN);
Button button4(BUTTON_4_PIN);
Button* inputButtonList[] = { &button1, &button2, &button3, &button4 };
ButtonGroup inputButtons(inputButtonList, MAX_SLOTS);

Adafruit_NeoPixel scanningStrip(MAX_SLOTS, SCANNING_LED_PIN); // TODO led type
Adafruit_NeoPixel feedbackStrip(FEEDBACK_LED_COUNT, FEEDBACK_LED_PIN); // TODO led type

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

GameHardware gameHardware(lcd, inputButtons, configButtons, steppers, audio, scanningStrip, feedbackStrip, vibration, VIBRATION_SUCCESS, VIBRATION_SUCCESS_LEN, VIBRATION_FAIL, VIBRATION_FAIL_LEN);

GameRunner runner(gameHardware);

Task* taskList[] = { &configButtons, &inputButtons, &runner, &vibration };
TaskGroup tasks(taskList, 4);

MenuHardware menuHardware(lcd, buttonUp, buttonSelect, buttonDown);

void startGame() {
    runner.startGame(options);
}
bool visualSubMenuHidden() {
    return options.visual != 1;
}
bool audioSubMenuHidden() {
    return options.audio != 1;
}
bool tactileSubMenuHidden() {
    return options.tactile != 1;
}

MenuRow* configRows[] = {
    new MenuOptionRow(menuHardware, &options.rounds, "rounds", OPTS_ROUNDS, 4),
    new MenuOptionRow(menuHardware, &options.slotsCount, "slots", OPTS_SLOTS, 2),
    new MenuOptionRow(menuHardware, &options.inputMode, "input mode", OPTS_INPUT_MODE, 2),

    new MenuOptionRow(menuHardware, &options.visual, "visual", OPTS_DIFFS, VISUAL_FEATS_COUNT + 2),
    new MenuOptionRow(menuHardware, &options.visualOptions[0], "  shape", OPTS_ON_OFF, 2, visualSubMenuHidden),
    new MenuOptionRow(menuHardware, &options.visualOptions[1], "  colour", OPTS_ON_OFF, 2, visualSubMenuHidden),
    new MenuOptionRow(menuHardware, &options.visualOptions[2], "  size", OPTS_ON_OFF, 2, visualSubMenuHidden),

    new MenuOptionRow(menuHardware, &options.audio, "audio", OPTS_DIFFS, AUDIO_FEATS_COUNT + 2),
    new MenuOptionRow(menuHardware, &options.audioOptions[0], "  loudness", OPTS_ON_OFF, 2, audioSubMenuHidden),
    new MenuOptionRow(menuHardware, &options.audioOptions[1], "  pitch", OPTS_ON_OFF, 2, audioSubMenuHidden),
    new MenuOptionRow(menuHardware, &options.audioOptions[2], "  timbre", OPTS_ON_OFF, 2, audioSubMenuHidden),
    new MenuOptionRow(menuHardware, &options.audioOptions[3], "  L/R", OPTS_ON_OFF, 2, audioSubMenuHidden), // or "panning"

    new MenuOptionRow(menuHardware, &options.tactile, "tactile", OPTS_DIFFS, TACTILE_FEATS_COUNT + 2),
    new MenuOptionRow(menuHardware, &options.tactileOptions[0], "  texture", OPTS_ON_OFF, 2, tactileSubMenuHidden),
    new MenuOptionRow(menuHardware, &options.tactileOptions[2], "  temperature", OPTS_ON_OFF, 2, tactileSubMenuHidden),
    
    new MenuActionRow(menuHardware, "Start game", startGame)
};
Menu configMenu(menuHardware, configRows, sizeof(configRows) / sizeof(configRows[0]));

void resetGame() {
    runner.reset();
}

char roundInfo[20] = {};

MenuRow* roundRows[] = {
    new MenuInfoRow(menuHardware, roundInfo),
    new MenuActionRow(menuHardware, "Reset/Restart Game", resetGame)
};
Menu roundMenu(menuHardware, roundRows, sizeof(roundRows) / sizeof(roundRows[0]));

char scoreInfo[20] = {};

void nextRound() {
    runner.nextRound();
}

MenuRow* nextRows[] = {
    new MenuInfoRow(menuHardware, roundInfo),
    new MenuInfoRow(menuHardware, scoreInfo),
    new MenuActionRow(menuHardware, "Next round", nextRound)
};
Menu nextMenu(menuHardware, nextRows, sizeof(nextRows) / sizeof(nextRows[0]), 2);

char endScoreInfo[20] = {};

void newGame() {
    runner.reset();
}

MenuRow* endRows[] = {
    new MenuInfoRow(menuHardware, endScoreInfo),
    new MenuActionRow(menuHardware, "New game", newGame)
};
Menu endMenu(menuHardware, endRows, sizeof(endRows) / sizeof(endRows[0]), 1);

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
    expander.begin_I2C(MCP_ADDR);

    tasks.begin();
    steppers.begin();
    audio.begin();
    audio.setVolume(AUDIO_VOLUME);

    lcd.begin();
}

void loop() {
    static bool selected = false;
    static uint8_t prev = 0;
    static uint8_t prev2 = 0;
    if (timeForNextUpdate()) {
        tasks.update();

        if (runner.stage().is(GameStage::CONFIG)) {
            menus.use(&configMenu);
        } else if (runner.stage().is(GameStage::SPINNING) || runner.stage().is(GameStage::SELECTION)) {
            if (runner.stage().changed()) {
                sprintf(roundInfo, "Round %d/%d", runner.currRound() + 1, runner.totalRounds());
            }
            menus.use(&roundMenu);
        } else if (runner.stage().is(GameStage::FEEDBACK)) {
            if (runner.stage().changed()) {
                sprintf(scoreInfo, "Total score: %d", runner.score());
                sprintf(endScoreInfo, "Total score: %d/%d", runner.score(), runner.totalRounds());
            }
            
            if (runner.hasNextRound()) {
                menus.use(&nextMenu);
            } else {
                menus.use(&endMenu);
            }
        } else {
            menus.use(nullptr);
        }
    }
    steppers.update();
    audio.update();
}
