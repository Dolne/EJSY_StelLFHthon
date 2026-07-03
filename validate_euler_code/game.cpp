#include "game.h"
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
    // generate 2 values: the odd 1 out, and the value for the other slots
    uint8_t odd1Out = random(1 << featuresCount); // 2^n
    uint8_t other = odd1Out;
    // the other value is the odd 1 out but with all the enabled features toggled
    for (int i = 0; i < featuresCount; i++) {
        if (features[i] == 1) {
            other ^= (1 << i); // flip the bit corresponding to the feature
        }
    }
    for (int i = 0; i < MAX_SLOTS; i++) {
        if (i < slotCount) {
            // add 1 to the index value generated as 0 represents null
            if (i == odd1OutSlot) {
                slots[i] = odd1Out + 1;
            } else {
                slots[i] = other + 1;
            }
        } else {
            slots[i] = 0;
        }
    }
}

GameRound::GameRound(GameOptions opts):
    slotsCount(opts.slotsCount + MIN_SLOTS),
    odd1OutSlot(random(opts.slotsCount)),
    hasVisual(opts.visual > 0),
    hasAudio(opts.audio > 0),
    hasTactile(opts.tactile > 0)
{
    if (hasVisual) {
        generateSlots(visual, slotsCount, odd1OutSlot, opts.visual, opts.visualOptions, VISUAL_FEATS_COUNT);
    }

    if (hasAudio) {
        generateSlots(audio, slotsCount, odd1OutSlot, opts.audio, opts.audioOptions, AUDIO_FEATS_COUNT);
    }

    if (hasTactile) {
        generateSlots(tactile, slotsCount, odd1OutSlot, opts.tactile, opts.tactileOptions, TACTILE_FEATS_COUNT);
    }
}
