#include "runner.h"

GameRunner::GameRunner(const GameHardware &hardware):
    hardware_(hardware),
    scanningRunner_(hardware)
{
}

void GameRunner::startGame(GameOptions opts)
{
    // if running, stop first
    reset();

    // set the game round
    options_ = opts;
    options_.enabled = true;
    gameStage_.set(GameStage::STARTING);
}

void GameRunner::nextRound()
{
    if (hasNextRound()) {
        rounds_++;
        gameStage_.set(GameStage::STARTING);
    } else {
        reset();
    }
}

void GameRunner::reset()
{
    options_.enabled = false;
    scanningRunner_.stop();
    delete round_;
    round_ = nullptr;
    score_ = 0;
    rounds_ = 0;
    gameStage_.set(GameStage::STOPPING);
}

void GameRunner::spinSteppers()
{
    if (round_ != 0) {
        for (int i = 0; i < MAX_SLOTS; i++) {
            float val = round_->visual[i];
            float rot = val / VISUAL_TOTAL_VALUES;
            Serial.print("stepper");
            Serial.print(i+1);
            Serial.print(": ");
            Serial.println(rot);
            Stepper* stepper = hardware_.steppers.get(i);
            if (val < VISUAL_VALUE_OFFSET) {
                // go direct
                stepper->directTo(rot);
            } else {
                // spinny spin based on i
                stepper->spinTo(rot, 5 + i);
            }
        }
    }
}
const State<GameStage>& GameRunner::stage()
{
    return gameStage_;
}

void GameRunner::begin()
{
    gameStage_.set(GameStage::CONFIG);
    scanningRunner_.begin();
}
void GameRunner::update()
{
    gameStage_.update();

    if (!gameStage_.is(GameStage::FEEDBACK) && gameStage_.changed()) {
        hardware_.vibration.disable();
        // TODO stop led animation
    }
    if (gameStage_.is(GameStage::STOPPING)) {
        if (gameStage_.changed()) {
            delete round_;
            round_ = 0;
            hardware_.steppers.stopAll();
            hardware_.audio.stop();
        } else {
            // also wait for audio to stop
            if (!hardware_.steppers.anyRunning() && !hardware_.audio.playing()) {
                gameStage_.set(GameStage::STOPPED);
            }
        }
    } else if (gameStage_.is(GameStage::STOPPED)) {
        if (options_.enabled) {
            gameStage_.set(GameStage::STARTING);
        } else {
            gameStage_.set(GameStage::CONFIG);
        }
    } else if (gameStage_.is(GameStage::CONFIG)) {
        if (gameStage_.changed()) {
            options_.enabled = true;
        }
    } else if (gameStage_.is(GameStage::STARTING)) {
        if (options_.enabled) {
            delete round_;
            round_ = new GameRound(options_);
            // maybe play some info audio first?
            Serial.println("Game starting...");
            printGameRound(round_);
            gameStage_.set(GameStage::SPINNING);
        } else {
            Serial.println("[ERR] Game stage set to STARTING without setting options");
            gameStage_.set(GameStage::CONFIG);
        }
    } else if (gameStage_.is(GameStage::SPINNING)) {
        if (gameStage_.changed()) {
            // start steppers
            Serial.println("Spinning steppers...");
            spinSteppers();
        } else { // else if steppers stopped
            if (!hardware_.steppers.anyRunning()) {
                gameStage_.set(GameStage::SELECTION);
            }
        }
    } else if (gameStage_.is(GameStage::SELECTION)) {
        if (gameStage_.changed()) {
            if (round_->inputMode == INPUT_MODE_SCANNING || round_->hasAudio) {
                scanningRunner_.startScanning(round_);
            } else if (round_->inputMode == INPUT_MODE_SELECT) {
                // do nothing??
            }
        }
        scanningRunner_.update();
        if (round_->inputMode == INPUT_MODE_SCANNING) {
            if (hardware_.inputButtons.anyToggled(true)) {
                round_->answer = scanningRunner_.slot();
                gameStage_.set(GameStage::FEEDBACK);
            }
        } else if (round_->inputMode == INPUT_MODE_SELECT) {
            for (int i = 0; i < round_->slotsCount; i++) {
                if (hardware_.inputButtons.get(i)->toggled(true)) {
                    round_->answer = i;
                    gameStage_.set(GameStage::FEEDBACK);
                    break;
                }
            }
        }
    } else if (gameStage_.is(GameStage::FEEDBACK)) {
        if (round_->answer >= round_->slotsCount) {
            // invalid answer; go back to answer selection
            gameStage_.set(GameStage::SELECTION);
        }
        if (gameStage_.changed()) {
            Serial.print("Slot selected: ");
            Serial.println(round_->answer);
            // update score, play audio
            if (round_->answer == round_->odd1OutSlot) {
                Serial.println("Correct answer!");
                score_++;
                // hardware_.audio.play(1, 5);
                hardware_.vibration.startSequence(hardware_.vibrationSeqSuccess, hardware_.vibrationSeqSuccessLen);
                // TODO start led success animation
            } else {
                Serial.println("Wrong answer :(");
                // hard
                // hardware_.audio.play(1, 6);
                hardware_.vibration.startSequence(hardware_.vibrationSeqFail, hardware_.vibrationSeqFailLen);
                // TODO start led failure animation
            }
        }
        // run led animation
        // should be plenty good enough to run in update here (100 times per second)
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


ScanningRunner::ScanningRunner(const GameHardware &hardware):
    hardware_(hardware),
    slot_(-1)
{
    scanStage_.set(ScanStage::STOPPED);
}

void ScanningRunner::startScanning(GameRound *round)
{
    slot_ = 0;
    round_ = round;
    scanStage_.set(ScanStage::SLOT_START);
}

void ScanningRunner::stop()
{
    scanStage_.set(ScanStage::STOPPED);
}

void ScanningRunner::begin()
{
}

void ScanningRunner::update()
{
    scanStage_.update();

    if (scanStage_.is(ScanStage::STOPPED)) {
        if (scanStage_.changed()) {
            slot_ = 0;
            hardware_.audio.stop();
            // probably doesnt need to wait for it to stop?
            // if need to wait should have a STOPPING state
            // TODO turn off led
        }
    } else if (scanStage_.is(ScanStage::WAITING)) {
        long diff = millis() - scanStage_.since();
        if (diff >= waitDuration_) {
            Serial.print("waited for ");
            Serial.println(diff);
            waitDuration_ = 0;
            scanStage_.set(waitNext_);
        }
    } else if (scanStage_.is(ScanStage::SLOT_START)) {
        Serial.print("Scanning slot ");
        Serial.println(slot_);
        // TODO turn on the arrow light for the slot
        scanStage_.set(ScanStage::INITIAL_AUDIO);
    } else if (scanStage_.is(ScanStage::INITIAL_AUDIO)) {
        if (scanStage_.changed()) {
            // if enabled, play audio
            // hardware_.audio.play(1, slot_ + 1);
        } else if (/* !hardware_.audio.playing() */ true) {
            if (round_->hasAudio) {
                wait(500, ScanStage::AUDIO);
            } else {
                wait(2000, ScanStage::SLOT_END);
            }
        }
    } else if (scanStage_.is(ScanStage::AUDIO)) {
        if (scanStage_.changed()) {
            // play audio
            if (round_->audio[slot_] > 0) {
                Serial.print("Playing audio ");
                Serial.println(round_->audio[slot_]);
                hardware_.audio.play(1, round_->audio[slot_]);
            }
        } else if (!hardware_.audio.playing()) {
            Serial.println("audio completed!");
            long dur = millis() - scanStage_.since();
            if (dur < 2000) {
                wait(2500 - dur, ScanStage::SLOT_END);
            } else {
                wait(500, ScanStage::SLOT_END);
            }
        }
    } else if (scanStage_.is(ScanStage::SLOT_END)) {
        // TODO turn off led
        if (round_->slotsCount <= MAX_SLOTS && slot_ < round_->slotsCount - 1) {
            slot_++;
        } else {
            slot_ = 0;
        }
        scanStage_.set(ScanStage::SLOT_START);
    }
}

uint8_t ScanningRunner::slot()
{
    return slot_;
}

void ScanningRunner::wait(long duration, ScanStage next)
{
    waitDuration_ = duration;
    waitNext_ = next;
    scanStage_.set(ScanStage::WAITING);
}
