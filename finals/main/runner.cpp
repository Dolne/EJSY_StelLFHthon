#include "runner.h"

const uint8_t SYSTEM_AUDIO_FOLDER = 10;
const uint8_t SUCCESS_AUDIO_INDEX = 151;
const uint8_t FAIL_AUDIO_INDEX = 152;

void successAnimation(long start, CRGB *leds, int n)
{
    static const int DURATION = 2000;
    int s = (millis() - start) % DURATION * 255 / DURATION;
    fill_rainbow(leds, n, s, 255 / n);
}

void failAnimation(long start, CRGB *leds, int n)
{
    static const int DURATION = 2000;
    float t = (millis() - start) % DURATION / (float)DURATION * 2;
    if (t > 1)
    {
        t = 2 - t;
    }
    fill_solid(leds, n, CRGB(255 * t, 0, 0));
}

GameRunner::GameRunner(const GameHardware &hardware) : hardware_(hardware),
                                                       scanningRunner_(hardware)
{
}

void GameRunner::startGame(GameOptions opts)
{
    // if running, stop first
    reset(); // sets to game stage to STOPPING

    // set the game options
    // this creates a copy of a game options to prevent from being changed externally while the game is running
    options_ = opts;
    options_.enabled = true;
    // game stage will be set to STARTING after STOPPED (after STOPPING)
}

void GameRunner::nextRound()
{
    if (hasNextRound())
    {
        // start the next round
        rounds_++;
        gameStage_.set(GameStage::STARTING);
    }
    else
    {
        // end the game if it is already the last round
        reset();
    }
}

void GameRunner::reset()
{
    // disable the options until new options provided from startGame
    options_.enabled = false;

    // ensure scanning runner is also stopped
    scanningRunner_.stop();

    // remove current game round and ensure memory is freed
    delete round_;
    round_ = nullptr;

    // reset all stats
    score_ = 0;
    rounds_ = 0;

    // trigger the STOPPING stage to stop all hardware
    gameStage_.set(GameStage::STOPPING);
}

void GameRunner::spinSteppers()
{
    if (round_ != nullptr)
    {
        // if visual is not enabled, all slots will have a value of 0
        // any unused slots will also be 0
        // it is still necessary to spin to the 0 position (blank slot)
        for (int i = 0; i < MAX_SLOTS; i++)
        {
            // the position to go to is represented by a fraction of a full rotation
            float val = round_->visual[i];
            float rot = val / VISUAL_TOTAL_VALUES;

            Serial.print("stepper");
            Serial.print(i + 1);
            Serial.print(": ");
            Serial.println(rot);
            Stepper *stepper = hardware_.steppers.get(i);

            // the first VISUAL_VALUE_OFFSET slots are "special" slots
            if (val < VISUAL_VALUE_OFFSET)
            {
                // go direct to blank slot or display slot
                stepper->directTo(rot);
            }
            // the rest of slots are the shapes
            else
            {
                // spinny spin based on i
                // this is for spinning to a shape when visual is enabled
                stepper->spinTo(rot, 5 + i);
            }
        }
    }
}
const State<GameStage> &GameRunner::stage()
{
    return gameStage_;
}

void GameRunner::begin()
{
    // show the game options menu at the start
    gameStage_.set(GameStage::CONFIG);
    scanningRunner_.begin();
}
void GameRunner::update()
{
    gameStage_.update();

    // show debug menu
    // the rest of the game logic will not run when in this stage
    // but this will not stop the hardware
    if (gameStage_.is(GameStage::DEBUG_MENU))
    {
        // exit debug menu if debug button pressed again
        if (hardware_.debugButton.toggled(true))
        {
            exitDebug();
        }
        // pause all game logic
        // debug menu is handled from main.cpp
        return;
    }

    // trigger debug menu if the debug button (BOOT) is pressed
    else if (hardware_.debugButton.toggled(true))
    {
        Serial.println("entering debug menu");
        enterDebug();
        return;
    }

    // ensure scanning runner is stopped if not in selection stage
    // and turn off the scanning strip
    if (!gameStage_.is(GameStage::SELECTION) && gameStage_.changed())
    {
        scanningRunner_.stop();
        scanningRunner_.update();
        fill_solid(hardware_.scanningLeds, hardware_.scanningLedCount, CRGB::Black);
    }

    // when reset is called
    if (gameStage_.is(GameStage::STOPPING))
    {
        if (gameStage_.changed())
        {
            // delete the current game round and reset steppers to 0
            delete round_;
            round_ = nullptr;
            hardware_.steppers.allDirectTo(0);
            hardware_.audio.stop();
        }

        if (options_.enabled)
        {
            // wait until hardware is stopped before starting a new game
            if (!hardware_.steppers.anyRunning() && !hardware_.audio.playing())
            {
                gameStage_.set(GameStage::STARTING);
            }
        }
        else
        {
            gameStage_.set(GameStage::CONFIG);
        }
    }

    // 1. show game options menu
    else if (gameStage_.is(GameStage::CONFIG))
    {
        // game runner just waits for user to start game
    }

    // 2. start the game
    else if (gameStage_.is(GameStage::STARTING))
    {
        // check that startGame was actually called
        if (options_.enabled)
        {
            Serial.println("Round starting...");

            // generate a new game round based on game options
            delete round_;
            round_ = new GameRound(options_);
            printGameRound(round_);

            // set the speaker volume
            uint8_t vol = VOLUME_VALUES[options_.volume];
            Serial.print("Setting volume to ");
            Serial.println(vol);
            hardware_.audio.setVolume(vol);
            // maybe play some info audio first?

            // start spinning the steppers as necessary
            gameStage_.set(GameStage::SPINNING);
        }
        // otherwise go back to game options menu
        else
        {
            Serial.println("[ERR] Game stage set to STARTING without setting options");
            gameStage_.set(GameStage::CONFIG);
        }
    }

    // 3. spin the steppers
    else if (gameStage_.is(GameStage::SPINNING))
    {
        if (gameStage_.changed())
        {
            // start steppers
            Serial.println("Spinning steppers...");
            spinSteppers();
        }
        else
        {
            // wait for steppers to stop, then go to user selection
            if (!hardware_.steppers.anyRunning())
            {
                gameStage_.set(GameStage::SELECTION);
            }
        }
    }

    // 4. start user selection, either scanning or select mode
    // this is also whether audio will be played
    else if (gameStage_.is(GameStage::SELECTION))
    {
        if (gameStage_.changed() && !round_->isNullRound)
        {
            if (round_->inputMode == INPUT_MODE_SCANNING)
            {
                // start the scanning runner
                int dur = SCAN_SPEED_VALUES[options_.scanSpeed];
                Serial.print("Starting scanning with duration of ");
                Serial.print(dur);
                Serial.println("ms");
                scanningRunner_.startScanning(round_, dur);
            }
            else if (round_->hasVisual)
            {
                // illuminate all visual options in select mode
                fill_solid(hardware_.scanningLeds, round_->slotsCount, SCAN_COLOUR);
            }
        }

        // run the scanning runner if it is started
        // the scanning runner is responsible for playing audio
        scanningRunner_.update();

        // check for user inputs
        if (round_->isNullRound)
        {
            // null gameplay means any button press is the "correct answer"
            if (hardware_.inputButtons.anyToggled(true))
            {
                round_->answer = round_->odd1OutSlot;
            }
        }
        else if (round_->inputMode == INPUT_MODE_SCANNING)
        {
            // in scanning mode, any button pressed will select the currently scanned slot
            if (hardware_.inputButtons.anyToggled(true))
            {
                round_->answer = scanningRunner_.slot();
            }
        }
        else if (round_->inputMode == INPUT_MODE_SELECT)
        {
            // in select mode, the button pressed will select its corresponding slot
            for (int i = 0; i < round_->slotsCount; i++)
            {
                if (hardware_.inputButtons.get(i)->toggled(true))
                {
                    round_->answer = i;

                    // assumes that only 1 button has just been pressed
                    // which is likely the case unless 2 buttons were pressed within a few milliseconds
                    break;
                }
            }
        }

        // if a valid answer has been provided, go to the feedback stage
        if (round_->answer < round_->slotsCount)
        {
            gameStage_.set(GameStage::FEEDBACK);
        }
    }

    // 5. show feedback based on user answer
    // this will show a menu to either go to next round or start new game
    // or retry the round is answer is wrong and retries enabled
    else if (gameStage_.is(GameStage::FEEDBACK))
    {
        // check for invalid answer
        if (round_->answer >= round_->slotsCount)
        {
            // go back to answer selection
            gameStage_.set(GameStage::SELECTION);
        }

        if (gameStage_.changed())
        {
            Serial.print("Slot selected: ");
            Serial.print(round_->answer);
            Serial.print(", Answer: ");
            Serial.println(round_->odd1OutSlot);

            // update score, play audio
            if (round_->answer == round_->odd1OutSlot)
            {
                Serial.println("Correct answer!");
                score_++;
                // show success icon
                for (int i = 0; i < round_->slotsCount; i++) {
                    hardware_.steppers.get(i)->directTo(1.0f / VISUAL_TOTAL_VALUES);
                }
                // start vibration
                hardware_.vibration.startSequence(hardware_.vibrationSeqSuccess, hardware_.vibrationSeqSuccessLen);
            }
            else
            {
                Serial.println("Wrong answer :(");
                // start vibration
                hardware_.vibration.startSequence(hardware_.vibrationSeqFail, hardware_.vibrationSeqFailLen);
            }
        }
        // once the steppers have stopped and delay has passed, play the audio
        else if (!hardware_.steppers.anyRunning() && millis() - gameStage_.since() > 500)
        {
            gameStage_.set(GameStage::FEEDBACK_AUDIO);
        }
    }

    // 6. play audio feedback based on user answer
    // this is triggered after some delay or after the steppers stop moving
    else if (gameStage_.is(GameStage::FEEDBACK_AUDIO))
    {
        if (gameStage_.changed())
        {
            if (round_->answer == round_->odd1OutSlot)
            {
                Serial.printf("Playing success audio (%02d, %03d)", SYSTEM_AUDIO_FOLDER, SUCCESS_AUDIO_INDEX);
                Serial.println();
                // play audio for success
                hardware_.audio.play(SYSTEM_AUDIO_FOLDER, SUCCESS_AUDIO_INDEX);
            }
            else
            {
                Serial.printf("Playing fail audio (%02d, %03d)", SYSTEM_AUDIO_FOLDER, FAIL_AUDIO_INDEX);
                Serial.println();
                // play audio for fail
                hardware_.audio.play(SYSTEM_AUDIO_FOLDER, FAIL_AUDIO_INDEX);
            }
        }
    }

    // play the led animations bsaed on user answer
    if (gameStage_.is(GameStage::FEEDBACK) || gameStage_.is(GameStage::FEEDBACK_AUDIO))
    {
        // run led animation
        if (round_->answer == round_->odd1OutSlot)
        {
            successAnimation(gameStage_.since(), hardware_.feedbackLeds, hardware_.feedbackLedCount);
            if (!hardware_.steppers.anyRunning()) {
                fill_solid(hardware_.scanningLeds, round_->slotsCount, SCAN_COLOUR);
            }
        }
        else
        {
            failAnimation(gameStage_.since(), hardware_.feedbackLeds, hardware_.feedbackLedCount);
        }
    }
    // disable feedback stage hardware if not in a feedback stage
    else if (gameStage_.changed())
    {
        hardware_.vibration.disable();
        hardware_.audio.stop();
        // hide led animation
        fill_solid(hardware_.feedbackLeds, hardware_.feedbackLedCount, CRGB::Black);
    }
}

uint8_t GameRunner::score()
{
    return score_;
}
uint8_t GameRunner::currRound()
{
    return rounds_;
}
uint8_t GameRunner::totalRounds()
{
    return options_.enabled ? options_.rounds + 1 : 0;
}

bool GameRunner::hasNextRound()
{
    return currRound() < totalRounds() - 1;
}

uint8_t GameRunner::roundAnswer()
{
    // null gameplay round still stores an unused answer internally so need to make sure not to return that
    if (round_ != nullptr && !round_->isNullRound)
    {
        return round_->odd1OutSlot;
    }
    else
    {
        return 0xFF;
    }
}

bool GameRunner::canRetry()
{
    // can only retry from FEEDBACK stage and only if retries are on and answer is wrong
    return (gameStage_.is(GameStage::FEEDBACK) || gameStage_.is(GameStage::FEEDBACK_AUDIO)) && options_.enabled && options_.retries && round_->answer != round_->odd1OutSlot;
}

void GameRunner::retry()
{
    if (canRetry())
    {
        // reset answer and go back to SELECTION stage
        round_->answer = 0xFF;
        gameStage_.set(GameStage::SELECTION);
    }
}

void GameRunner::enterDebug()
{
    gameStage_.set(GameStage::DEBUG_MENU);
}

void GameRunner::exitDebug()
{
    if (gameStage_.is(GameStage::DEBUG_MENU))
    {
        gameStage_.revert();
    }
}

ScanningRunner::ScanningRunner(const GameHardware &hardware) : hardware_(hardware),
                                                               slot_(0xFF)
{
    scanStage_.set(ScanStage::STOPPED);
}

void ScanningRunner::startScanning(GameRound *round, int scanDuration)
{
    slot_ = 0;
    round_ = round;
    scanDuration_ = scanDuration;
    // go to STOPPED stage first to reset everything before starting
    scanStage_.set(ScanStage::STOPPED);
}

void ScanningRunner::stop()
{
    round_ = nullptr;
    scanStage_.set(ScanStage::STOPPED);
}

void ScanningRunner::begin()
{
}

void ScanningRunner::update()
{
    scanStage_.update();

    if (round_ == nullptr && !scanStage_.is(ScanStage::STOPPED))
    {
        scanStage_.set(ScanStage::STOPPED);
        return;
    }

    // when runner is stopped externally
    // this is the default state when the runner is not running
    if (scanStage_.is(ScanStage::STOPPED))
    {
        if (scanStage_.changed())
        {
            slot_ = 0;
            hardware_.audio.stop();
            fill_solid(hardware_.scanningLeds, hardware_.scanningLedCount, CRGB::Black);
        }
        if (round_ != nullptr)
        {
            scanStage_.set(ScanStage::SLOT_START);
        }
    }

    // special stage triggered by `wait` function
    else if (scanStage_.is(ScanStage::WAITING))
    {
        // duration is calculated based on when the WAITING stage was started
        long diff = millis() - scanStage_.since();
        if (diff >= waitDuration_)
        {
            Serial.print("waited for ");
            Serial.println(diff);
            waitDuration_ = 0;
            // go to the specified stage once wait is over
            scanStage_.set(waitNext_);
        }
    }

    // 1. at the start of scanning each slot, turn on the scanning led
    else if (scanStage_.is(ScanStage::SLOT_START))
    {
        slotStart_ = millis();
        Serial.print("Scanning slot ");
        Serial.println(slot_);
        // turn on the arrow light for the slot
        hardware_.scanningLeds[slot_] = SCAN_COLOUR;
        scanStage_.set(ScanStage::INITIAL_AUDIO);
    }

    // 2. then play an audio with the slot number (currently unimplemented)
    else if (scanStage_.is(ScanStage::INITIAL_AUDIO))
    {
        if (scanStage_.changed())
        {
            // TODO play audio (maybe not)
            // hardware_.audio.play(1, slot_ + 1);
        }
        else if (!hardware_.audio.playing())
        {
            // some delay after playing the audio
            // mainly in case there is audio after this also
            wait(300, ScanStage::AUDIO);
        }
    }

    // 3. play the actual audio if audio is enabled
    else if (scanStage_.is(ScanStage::AUDIO))
    {
        if (scanStage_.changed())
        {
            // play audio if any
            if (round_->audio[slot_] > 0 && round_->audioFolder > 0)
            {
                Serial.printf("Playing audio (%02d, %03d)", round_->audioFolder, round_->audio[slot_]);
                Serial.println();
                if (SIMULATION)
                {
                    Serial.println("(simulated)");
                }
                else
                {
                    hardware_.audio.play(round_->audioFolder, round_->audio[slot_]);
                }
            }
        }
        // once audio finishes playing, or audio didnt play at all
        else if (!hardware_.audio.playing())
        {
            Serial.println("waiting...");
            // wait until scan duration is finished
            wait(scanDuration_ - (millis() - slotStart_), ScanStage::SLOT_END);
        }
    }

    // 4. at the end of scanning the slot, turn off the scanning led and go to the next slot
    else if (scanStage_.is(ScanStage::SLOT_END))
    {
        // turn off led
        fill_solid(hardware_.scanningLeds, hardware_.scanningLedCount, CRGB::Black);

        // increment to the next slot or wrap over back to the first slot
        if (round_->slotsCount <= MAX_SLOTS && slot_ < round_->slotsCount - 1)
        {
            slot_++;
        }
        else
        {
            slot_ = 0;
        }

        // some delay between each slot to prevent user accidently pressing for the next slot
        wait(500, ScanStage::SLOT_START);
    }
}

uint8_t ScanningRunner::slot()
{
    // return "none" (0xFF) if in between slots
    if (scanStage_.is(ScanStage::WAITING) && waitNext_ == ScanStage::SLOT_START)
    {
        return 0xFF;
    }
    return slot_;
}

void ScanningRunner::wait(long duration, ScanStage next)
{
    // set the variables here, actual waiting is implemented in `update` under the WAITING stage
    waitDuration_ = duration;
    waitNext_ = next;
    scanStage_.set(ScanStage::WAITING);
}