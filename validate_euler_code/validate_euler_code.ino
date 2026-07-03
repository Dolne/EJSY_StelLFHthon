#include <Arduino.h>
#include <Wire.h>

#include "hardware.h"
#include "task.h"
#include "menu.h"
#include "game.h"

const int lcdAddr = 0x27;
const int buttonUpPin = 18;
const int buttonSelectPin = 17;
const int buttonDownPin = 16;

const int TICKRATE = 100;

Button buttonUp(buttonUpPin);
Button buttonSelect(buttonSelectPin);
Button buttonDown(buttonDownPin);
Task* buttonList[] = { &buttonUp, &buttonSelect, &buttonDown };
TaskGroup buttons(buttonList, 3);

LCD lcd = LCD(lcdAddr);

MenuHardware hardware(lcd, buttonUp, buttonSelect, buttonDown);

GameOptions options{};

bool visualSubMenuHidden() {
    return options.visual != 1;
}
bool audioSubMenuHidden() {
    return options.audio != 1;
}
bool tactileSubMenuHidden() {
    return options.tactile != 1;
}

MenuRow* rows[] = {
    new MenuOptionRow(hardware, &options.rounds, "rounds", OPTS_ROUNDS, 4),
    new MenuOptionRow(hardware, &options.slotsCount, "slots", OPTS_SLOTS, 2),

    new MenuOptionRow(hardware, &options.visual, "visual", OPTS_DIFFS, VISUAL_FEATS_COUNT + 2),
    new MenuOptionRow(hardware, &options.visualOptions[0], "  shape", OPTS_ON_OFF, 2, visualSubMenuHidden),
    new MenuOptionRow(hardware, &options.visualOptions[1], "  colour", OPTS_ON_OFF, 2, visualSubMenuHidden),
    new MenuOptionRow(hardware, &options.visualOptions[2], "  size", OPTS_ON_OFF, 2, visualSubMenuHidden),

    new MenuOptionRow(hardware, &options.audio, "audio", OPTS_DIFFS, AUDIO_FEATS_COUNT + 2),
    new MenuOptionRow(hardware, &options.audioOptions[0], "  loudness", OPTS_ON_OFF, 2, audioSubMenuHidden),
    new MenuOptionRow(hardware, &options.audioOptions[1], "  pitch", OPTS_ON_OFF, 2, audioSubMenuHidden),
    new MenuOptionRow(hardware, &options.audioOptions[2], "  timbre", OPTS_ON_OFF, 2, audioSubMenuHidden),
    new MenuOptionRow(hardware, &options.audioOptions[3], "  L/R", OPTS_ON_OFF, 2, audioSubMenuHidden), // or "panning"

    new MenuOptionRow(hardware, &options.tactile, "tactile", OPTS_DIFFS, TACTILE_FEATS_COUNT + 2),
    new MenuOptionRow(hardware, &options.tactileOptions[0], "  texture", OPTS_ON_OFF, 2, tactileSubMenuHidden),
    new MenuOptionRow(hardware, &options.tactileOptions[2], "  temperature", OPTS_ON_OFF, 2, tactileSubMenuHidden),
    
    new MenuActionRow(hardware, "Start game")
};
Menu menu(hardware, rows, 15);

bool timeForNextUpdate() {
    static long nextTime = millis();
    long time = millis();
    if (time >= nextTime) {
        nextTime += 1000 / TICKRATE;
        return true;
    }
    return false;
}

void setup() {
    Serial.begin(9600);

    buttons.begin();

    lcd.begin();
}

void loop() {
    static bool selected = false;
    static uint8_t prev = 0;
    static uint8_t prev2 = 0;
    if (timeForNextUpdate()) {
        buttons.update();
        // actl are most buttons triggered on up (after down) rather then on down
        menu.update();
    }
}
