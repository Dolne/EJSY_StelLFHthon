#include "game.h"

/**
 * takes a random sample of size `enabled` and sets to 1, while all others set to 0.
 * this is used to randomly enable a specified number of features.
 */
void enableFeatures(uint8_t *features, uint8_t featuresCount, uint8_t enabled)
{
    uint8_t done = 0;
    for (int i = 0; i < featuresCount; i++)
    {
        if (random(featuresCount - i) < (enabled - done))
        {
            features[i] = 1;
            done++;
        }
        else
        {
            features[i] = 0;
        }
    }
}

bool hasFeatures(uint8_t *features, uint8_t featuresCount)
{
    for (int i = 0; i < featuresCount; i++)
    {
        if (features[i] != 0)
        {
            return true;
        }
    }
    return false;
}

/**
 * generate the index to display/play for each slot.
 */
bool generateSlots(uint8_t slots[4], uint8_t slotCount, uint8_t odd1OutSlot, uint8_t enabled, uint8_t *features, uint8_t featuresCount, uint8_t offset)
{
    // stimuli is not enabled (off)
    if (enabled == 0)
    {
        return false;
    }
    // stimuli is enabled with the number of features specified (1 diff, 2 diff, etc)
    else if (enabled > 1)
    {
        // need to randomly enable (n) features (ie choose which features will differ)
        enableFeatures(features, featuresCount, enabled - 1);
    }
    // otherwise it means stimuli is enabled with manually selected features (manual)
    // need to check if any features have actually been enabled manually
    else if (!hasFeatures(features, featuresCount))
    {
        return false;
    }

    // randomly generate the odd-1-out option
    uint8_t odd1Out = random(1 << featuresCount); // 0 <= odd1Out < 2^featuresCount

    // the other value is the odd-1-out but with all the enabled features toggled
    uint8_t other = odd1Out;
    for (int i = 0; i < featuresCount; i++)
    {
        if (features[i] == 1)
        {
            other ^= (1 << i); // flip the bit corresponding to the feature
        }
    }

    // populate the slots array with the index to show for each slot
    for (int i = 0; i < MAX_SLOTS; i++)
    {
        if (i < slotCount)
        {
            // add to the index value generated as 0 represents null
            // for visual 1 is also a special display slot
            if (i == odd1OutSlot)
            {
                slots[i] = odd1Out + offset;
            }
            else
            {
                slots[i] = other + offset;
            }
        }
        // can have unused slots based on number of slots chosen by gamemaster
        else
        {
            // 0 represents null (ie nothing will be displayed or played)
            slots[i] = 0;
        }
    }
    return true;
}

GameRound::GameRound(GameOptions opts) : slotsCount(opts.slotsCount + MIN_SLOTS),
                                         inputMode(opts.inputMode),
                                         odd1OutSlot(random(slotsCount)), // randomly select the slot to use as the odd one out
                                         hasVisual(opts.visual > 0),
                                         hasAudio(opts.audio > 0 && opts.inputMode == 0),    // audio can only be active if input mode is "scanning"
                                         hasTactile(opts.tactile > 0 && opts.inputMode == 1) // tactile can only be active if input mode is "select"
{
    // generate the options to present to the user in that round based on the game options

    if (hasVisual)
    {
        hasVisual = generateSlots(visual, slotsCount, odd1OutSlot, opts.visual, opts.visualOptions, VISUAL_FEATS_COUNT, VISUAL_VALUE_OFFSET);
    }

    if (hasAudio)
    {
        hasAudio = generateSlots(audio, slotsCount, odd1OutSlot, opts.audio, opts.audioOptions, AUDIO_FEATS_COUNT, AUDIO_VALUE_OFFSET);
    }

    // if there are no features enabled at all, it means it will be in the "null" gameplay mode
    isNullRound = !hasVisual && !hasAudio && !hasTactile;
}

void printSlots(uint8_t slots[4], uint8_t digits, uint8_t offset)
{
    Serial.print(" [");
    for (int i = 0; i < 4; i++)
    {
        Serial.print(slots[i]);
        Serial.print(" ");
        if (slots[i] >= offset)
        {
            Serial.print("(");
            uint8_t val = slots[i] - offset;
            for (int j = digits - 1; j >= 0; j--)
            {
                Serial.print(bitRead(val, j));
            }
            Serial.print(") ");
        }
    }
    Serial.println("]");
}

void printGameRound(GameRound *gameRound)
{
    Serial.print("slots: ");
    Serial.println(gameRound->slotsCount);
    Serial.print("odd 1 out: ");
    if (gameRound->isNullRound)
    {
        Serial.println("NULL");
    }
    else
    {
        Serial.println(gameRound->odd1OutSlot);
    }

    Serial.print("visual: ");
    Serial.print(gameRound->hasVisual);
    printSlots(gameRound->visual, VISUAL_FEATS_COUNT, VISUAL_VALUE_OFFSET);

    Serial.print("audio: ");
    Serial.print(gameRound->hasAudio);
    printSlots(gameRound->audio, AUDIO_FEATS_COUNT, AUDIO_VALUE_OFFSET);

    Serial.print("tactile: ");
    Serial.println(gameRound->hasTactile);
}

GameHardware::GameHardware(
    LCD &lcd,
    const ButtonGroup &inputButtons, const Button &debugButton,
    StepperGroup &steppers,
    AudioPlayer &audio,
    CRGB *scanningLeds, int scanningLedCount, CRGB *feedbackLeds, int feedbackLedCount,
    OutputController &vibration,
    const int *v1, int l1, const int *v2, int l2) : lcd(lcd),
                                                    inputButtons(inputButtons),
                                                    debugButton(debugButton),
                                                    steppers(steppers),
                                                    audio(audio),
                                                    scanningLeds(scanningLeds),
                                                    scanningLedCount(scanningLedCount),
                                                    feedbackLeds(feedbackLeds),
                                                    feedbackLedCount(feedbackLedCount),
                                                    vibration(vibration),
                                                    vibrationSeqSuccess(v1),
                                                    vibrationSeqSuccessLen(l1),
                                                    vibrationSeqFail(v2),
                                                    vibrationSeqFailLen(l2)
{
}
