#include "demo.h"

#define DEMO_MAX_FEAT 2;

DemoRunner::DemoRunner(GameHardware &hardware, const GameOptions &gameOpts) : hardware_(hardware), runner_(hardware_), gameOpts_(gameOpts)
{
}

void DemoRunner::reshuffle()
{
    if (!isActive())
    {
        return;
    }
    GameOptions opts{};

    opts.slotsCount = 1;

    // copy some options from the actual game options
    opts.audioFolder = gameOpts_.audioFolder;

    // visual
    if (currStimuli_ == 0)
    {
        opts.visual = 1;
        opts.visualOptions[currFeat_] = 1;
    }
    // audio
    else if (currStimuli_ == 1)
    {
        opts.audio = 1;
        opts.audioOptions[currFeat_] = 1;
    }

    delete round_;
    round_ = new GameRound(opts);
    printGameRound(round_);

    runner_.stop();
    fill_solid(hardware_.scanningLeds, hardware_.scanningLedCount, CRGB::Black);

    // visual
    if (currStimuli_ == 0)
    {
        for (int i = 0; i < MAX_SLOTS; i++)
        {
            float val = round_->visual[i];
            float rot = val / VISUAL_TOTAL_VALUES;
            hardware_.steppers.get(i)->directTo(rot);
        }
    }
    // audio
    else if (currStimuli_ == 1)
    {
        runner_.startScanning(round_, SCAN_SPEED_VALUES[gameOpts_.scanSpeed]);
    }
}

void DemoRunner::next()
{
    currFeat_++;

    // go to the next stimuli if the current one is done
    if ((currStimuli_ == 0 && currFeat_ >= VISUAL_FEATS_COUNT) || (currStimuli_ == 1 && currFeat_ >= AUDIO_FEATS_COUNT))
    {
        currStimuli_++;
        currFeat_ = 0;
    }

    stateChanged_ = true;
}

void DemoRunner::start()
{
    currStimuli_ = 0;
    currFeat_ = 0;
    stateChanged_ = true;

    // set audio volume
    uint8_t vol = VOLUME_VALUES[gameOpts_.volume];
    Serial.print("Setting volume to ");
    Serial.println(vol);
    hardware_.audio.setVolume(vol);
}

void DemoRunner::end()
{
    runner_.stop();
    fill_solid(hardware_.scanningLeds, hardware_.scanningLedCount, CRGB::Black);
    delete round_;
    round_ = nullptr;
    currStimuli_ = DEMO_MAX_FEAT;
    stateChanged_ = true;
}

bool DemoRunner::isActive()
{
    return currStimuli_ < DEMO_MAX_FEAT;
}

void DemoRunner::begin()
{
    runner_.begin();
    currStimuli_ = DEMO_MAX_FEAT;
}
void DemoRunner::update()
{
    nameChanged_ = false;
    if (stateChanged_)
    {
        if (currStimuli_ != 0)
        {
            hardware_.steppers.allDirectTo(0);
        }
        if (isActive())
        {
            sprintf(currName, "%d-%d%s", currStimuli_ + 1, currFeat_ + 1, currStimuli_ == 0 ? VISUAL_FEATS_NAMES[currFeat_] : AUDIO_FEATS_NAMES[currFeat_]);
            reshuffle();
        }
        else
        {
            sprintf(currName, "");
            end();
        }
        nameChanged_ = true;
        stateChanged_ = false;
    }

    runner_.update();

    if (currStimuli_ == 0 && !hardware_.steppers.anyRunning())
    {
        // turn on the scanning LEDs when steppers stop spinning
        fill_solid(hardware_.scanningLeds, round_->slotsCount, SCAN_COLOUR);
    }
}

bool DemoRunner::currNameChanged()
{
    return nameChanged_;
}