#include "hardware.h"

byte rightArrowChar[] = {
    0b00000,
    0b01000,
    0b01100,
    0b01110,
    0b01111,
    0b01110,
    0b01100,
    0b01000
};
byte buttonDownChar[] = {
	0b00100,
	0b00100,
	0b00100,
	0b11111,
	0b01110,
	0b00100,
	0b00000,
	0b11111
};
byte buttonUpChar[] = {
	0b00000,
	0b00000,
	0b00000,
	0b00000,
	0b00000,
	0b11111,
	0b11111,
	0b00000
};

LCD::LCD(uint8_t addr): lcd_(addr, 20, 4)
{
}

void LCD::begin()
{
    lcd_.init();
    lcd_.createChar(LCD_CHAR_ARROW, rightArrowChar);
    lcd_.createChar(LCD_CHAR_BUTTON_DOWN, buttonDownChar);
    lcd_.createChar(LCD_CHAR_BUTTON_UP, buttonUpChar);
    lcd_.backlight();
}

void LCD::print(uint8_t row, uint8_t col, char ch)
{
    lcd_.setCursor(col, row);
    lcd_.print(ch);
}
void LCD::print(uint8_t row, uint8_t col, uint8_t width, const char* str)
{
    lcd_.setCursor(col, row);
    char buffer[width + 1];
    buffer[width] = '\0';
    bool terminated = false;
    for (int i = 0; i < width; i++) {
        if (!terminated) {
            char c = str[i];
            if (c == '\0') {
                terminated = true;
            } else {
                buffer[i] = c;
            }
        }
        if (terminated) {
            buffer[i] = ' ';
        }
    }
    lcd_.print(buffer);
}
void LCD::clear()
{
    lcd_.clear();
}

HardwarePin::HardwarePin(uint8_t pin):
    pin_(pin)
{
}

HardwarePin::HardwarePin(uint8_t pin, Adafruit_MCP23XXX *expander):
    pin_(pin), expander_(expander)
{
}

void HardwarePin::pinMode(uint8_t mode)
{
    if (expander_ == nullptr) {
        ::pinMode(pin_, mode);
    } else {
        expander_->pinMode(pin_, mode);
    }
}

uint8_t HardwarePin::digitalRead() {
    if (expander_ == nullptr) {
        return ::digitalRead(pin_);
    } else {
        return expander_->digitalRead(pin_);
    }
}

void HardwarePin::digitalWrite(uint8_t val)
{
    if (expander_ == nullptr) {
        ::digitalWrite(pin_, val);
    } else {
        expander_->digitalWrite(pin_, val);
    }
}

AudioPlayer::AudioPlayer(HardwareSerial &serial, uint8_t rxPin, uint8_t txPin):
    serial_(serial),
    mp3_(serial_),
    rxPin_(rxPin),
    txPin_(txPin)
{
}

void AudioPlayer::begin()
{
    serial_.begin(MD_YX5300::SERIAL_BPS, SERIAL_8N1, rxPin_, txPin_);
    mp3_.begin();
    mp3_.setSynchronous(false);
}

void AudioPlayer::update()
{
    if (mp3_.check()) {
        handleStatus_(mp3_.getStatus());
    }
}

void AudioPlayer::play(uint8_t track)
{
    mp3_.playTrack(track);
    playing_ = true;
}
void AudioPlayer::play(uint8_t folder, uint8_t track)
{
    mp3_.playSpecific(folder, track);
    playing_ = true;
}

void AudioPlayer::stop()
{
    mp3_.playStop();
    playing_ = false;
}

void AudioPlayer::setVolume(uint8_t volume)
{
    mp3_.volume(volume);
}

bool AudioPlayer::playing()
{
    return playing_;
}

void AudioPlayer::handleStatus_(const MD_YX5300::cbData *data)
{
    switch (data->code) {
        case MD_YX5300::STS_ACK_OK:
            // last command acknolwedged; do nothing
            Serial.println("STS_ACK_OK");
            break;
        case MD_YX5300::STS_FILE_END:
            // file being played has ended; update status
            Serial.println("STS_FILE_END");
            playing_ = false;
            break;
        case MD_YX5300::STS_ERR_FILE:
            // error playing file; update status
            Serial.println("STS_ERR_FILE");
            playing_ = false;
            break;
    }
}

OutputController::OutputController(HardwarePin pin):
    pin_(pin)
{}
void OutputController::begin()
{
    pin_.pinMode(OUTPUT);
    pin_.digitalWrite(LOW);
}
void OutputController::update()
{
    long time = millis();
    if (seq_ != nullptr && seqLen_ > 0 && time >= seqStart_) {
        int t = (time - seqStart_) % seqDuration_;
        int accum = 0;
        for (int i = 0; i < seqLen_; i++) {
            accum += seq_[i];
            if (t < accum) {
                // currently this will digital write every update
                if (i % 2 == 0) {
                    pin_.digitalWrite(HIGH);
                } else {
                    pin_.digitalWrite(LOW);
                }
                break;
            }
        }
    }
}
void OutputController::enable()
{
    pin_.digitalWrite(HIGH);
}
void OutputController::disable()
{
    seq_ = nullptr;
    seqLen_ = 0;
    pin_.digitalWrite(LOW);
}
void OutputController::startSequence(const int seq[], int seqLen)
{
    seq_ = seq;
    seqLen_ = seqLen;
    seqStart_ = millis();
    seqDuration_ = 0;
    for (int i = 0; i < seqLen; i++) {
        seqDuration_ += seq[i];
    }
}
