# TODO list

should we use github issues & milestones instead

## Software

### Software ↔︎ Hardware
- [x] YX5300 (Audio Player)
- [x] validate audio ([validate_audio_alt](./validate_audio_alt/validate_audio_alt.ino)) and create audio files, then uncomment audio code
- [ ] ~~stepper homing sequence (light gates)~~
- [x] bring steppers to 0 at end of game or before turning off
- [x] periodically refresh entire screen

### Gamemaster
- [ ] ~~Show options for tactile on gamemaster screen~~
- [ ] ~~Check/validate options (eg no diffs selected)~~
- [x] customizable scanning speed
- [x] customizable audio volume
- [x] show answer slot
- [x] collapse tactile options

### User Controls
- [x] Individually addressable RGB for scanning
- [x] small delay between slots when scanning
- [x] let user try again if they get the ans wrong
- [x] if doing audio only, dont need show the "1" "2" "3" "4", but still need to light up each wheel
- [x] "null" gameplay if no diffs selected

### Feedback
- [x] Individually addressable RGB for feedback
- [x] Vibration Motor
- [x] Round / Score messages
- [ ] Win ("Yaay") /lose ("Please try again") audio
- [ ] wheels "success icon"

## Hardware

### Option stimuli
- [x] Individual neopixels / make some
- [x] YX5300 ~~& Logic Level Shifter (on breadboard) (don't seem to need the LLC actl, and it seems to make the YX TX --> ESP RX data not work)~~
- [ ] ~~LightGate - forgo for now since no time~~
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
- [x] ~~solder one more F 3.5mm jack --> just use the screw one~~
- [ ] ~~Velcro for icons on the wheel --> just use double sided tape actually works~~

### In order of importance
#### High Effort High Impact
- [ ] Box for each wheel
    - [ ] Cover up the visual icons that aren't being shown
- [ ] Box for electronics
- [ ] Give the wheel box and button the same number with the same text colour (just 2d print)
- [ ] Technical guide for AT staff on how to change visual (create strip of icons) and audio (Add MP3 files to SD Card)
- [ ] Gamemaster guide
- [ ] Our actual presentation

#### Low Effort High Impact
- [ ] The white button might hv some issues, check it
- [ ] Win ("Yaay") /lose ("Please try again") audio
- [ ] Rigid strip to mount feedback LED strip on

#### Low Effort Low Impact
- [ ] Prepare the strip for the wheel

#### High Effort Low Impact

### Done
- [x] For audio decide if they change lesson by changing SD Card or by pressing in gamemaster controls --> Change via software

- [ ] For audio decide if we will tell them the volume and left/right channel difference can be done by hardware (e.g. relays and change vol command to MP3 module)
- [ ] Add star icon to replace number icon on wheel


