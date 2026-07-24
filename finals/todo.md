# TODO list

should we use github issues & milestones instead

## Software

### Software ↔︎ Hardware
- [x] YX5300 (Audio Player)
- [x] validate audio ([validate_audio_alt](./validate_audio_alt/validate_audio_alt.ino)) and create audio files, then uncomment audio code
- [ ] stepper homing sequence (light gates)
- [x] bring steppers to 0 at end of game or before turning off
- [x] periodically refresh entire screen

### Gamemaster
- [ ] Show options for tactile on gamemaster screen
- [ ] Check/validate options (eg no diffs selected)
- [ ] customizable scanning speed

### User Controls
- [x] Individually addressable RGB for scanning
- [ ] small delay between slots when scanning?

### Feedback
- [x] Individually addressable RGB for feedback
- [x] Vibration Motor
- [x] Round / Score messages

## Hardware

### Option stimuli
- [x] Individual neopixels / make some
- [x] YX5300 ~~& Logic Level Shifter (on breadboard) (don't seem to need the LLC actl, and it seems to make the YX TX --> ESP RX data not work)~~
- [ ] LightGate - might forgo for now since no time
- [x] Overlays
- [x] Wheels
- [x] Stepper mount

### User Controls
- [x] Scanning RGB Lightstrip - have the LEDs, havent connected

### Feedback
- [x] Vibration Motor & Relay
- [x] Feedback Lightstrip - have the LEDs, havent connected

### Power
- [x] Power Solution
- [x] 5V Power Solution

### Others
- [x] Masking tape labels on the wires
- [ ] The white button might hv some issues, check it
- [ ] solder one more F 3.5mm jack