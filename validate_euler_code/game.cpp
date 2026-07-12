#include "game.h"
#include "hardware.h"
#include "menu.h"

/**
 * takes a random sample of size `enabled` and sets to 1, while all others set to 0.
 */
void enableFeatures(uint8_t* features, uint8_t featuresCount, uint8_t enabled) {
    uint8_t done = 0;
    for (int i = 0; i < featuresCount; i++) {
        if (random(featuresCount - i) < (enabled - done)) {
            features[i] = 1;
            done++;
        } else {
            features[i] = 0;
        }
    }
}

void generateSlots(uint8_t slots[4], uint8_t slotCount, uint8_t odd1OutSlot, uint8_t enabled, uint8_t* features, uint8_t featuresCount)
{
    if (enabled == 0) {
        // need to do anything? eg set slots to all NONE
        return;
    } else if (enabled > 1) {
        enableFeatures(features, featuresCount, enabled - 1);
    } else {
        // TODO check that at least 1 feature is enabled for manual
    }
    // generate 2 values: the odd-1-out, and the value for the other slots
    uint8_t odd1Out = random(1 << featuresCount); // 2^n
    uint8_t other = odd1Out;
    // the other value is the odd-1-out but with all the enabled features toggled
    for (int i = 0; i < featuresCount; i++) {
        if (features[i] == 1) {
            other ^= (1 << i); // flip the bit corresponding to the feature
        }
    }
    for (int i = 0; i < MAX_SLOTS; i++) {
        if (i < slotCount) {
            // add 2 to the index value generated as 0 represents null and 1 is the slot number display
            if (i == odd1OutSlot) {
                slots[i] = odd1Out + VISUAL_VALUE_OFFSET;
            } else {
                slots[i] = other + VISUAL_VALUE_OFFSET;
            }
        } else {
            slots[i] = 0;
        }
    }
}

GameRound::GameRound(GameOptions opts):
    slotsCount(opts.slotsCount + MIN_SLOTS),
    inputMode(opts.inputMode),
    odd1OutSlot(random(slotsCount)),
    hasVisual(opts.visual > 0),
    hasAudio(opts.audio > 0),
    hasTactile(opts.tactile > 0)
{
    if (hasVisual) {
        generateSlots(visual, slotsCount, odd1OutSlot, opts.visual, opts.visualOptions, VISUAL_FEATS_COUNT);
    } else {
        for (int i = 0; i < slotsCount; i++) {
            visual[i] = 1; // display the slot number
        }
    }

    if (hasAudio) {
        generateSlots(audio, slotsCount, odd1OutSlot, opts.audio, opts.audioOptions, AUDIO_FEATS_COUNT);
    }

    if (hasTactile) {
        generateSlots(tactile, slotsCount, odd1OutSlot, opts.tactile, opts.tactileOptions, TACTILE_FEATS_COUNT);
    }
}


void printSlots(uint8_t slots[4], uint8_t digits) {
    Serial.print(" [");
    for (int i = 0; i < 4; i++) {
        Serial.print(slots[i]);
        Serial.print(" (");
        uint8_t val = slots[i] - VISUAL_VALUE_OFFSET;
        for (int j = digits - 1; j >= 0; j--) {
            Serial.print(bitRead(val, j));
        }
        Serial.print(") ");
    }
    Serial.println("]");
}

void printGameRound(GameRound* gameRound)
{
    Serial.print("slots: ");
    Serial.println(gameRound->slotsCount);
    Serial.print("odd 1 out: ");
    Serial.println(gameRound->odd1OutSlot);

    Serial.print("visual: ");
    Serial.print(gameRound->hasVisual);
    printSlots(gameRound->visual, VISUAL_FEATS_COUNT);

    Serial.print("audio: ");
    Serial.print(gameRound->hasAudio);
    printSlots(gameRound->audio, AUDIO_FEATS_COUNT);

    Serial.print("tactile: ");
    Serial.print(gameRound->hasTactile);
    printSlots(gameRound->tactile, TACTILE_FEATS_COUNT);
}

GameHardware::GameHardware(LCD &lcd, const ButtonGroup &inputButtons, const ButtonGroup &configButtons, StepperGroup& steppers, AudioPlayer& audio):
    lcd(lcd),
    inputButtons(inputButtons),
    configButtons(configButtons),
    steppers(steppers),
    audio(audio)
{
}
