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
- [x] demo mode
- [x] disable lcd if steppers running
- [x] demo mode volume
- [x] customizable audio folder
- [ ] show error if audio not found?

### User Controls
- [x] Individually addressable RGB for scanning
- [x] small delay between slots when scanning
- [x] let user try again if they get the ans wrong
- [x] if doing audio only, dont need show the "1" "2" "3" "4", but still need to light up each wheel
- [x] "null" gameplay if no diffs selected
- [x] change scanning neopixel colour to white

### Feedback
- [x] Individually addressable RGB for feedback
- [x] small delay between slots when scanning
- [x] let user try again if they get the ans wrong
- [x] if doing audio only, dont need show the "1" "2" "3" "4", but still need to light up each wheel
- [x] "null" gameplay if no diffs selected
- [x] Vibration Motor
- [x] Round / Score messages
- [x] Win ("Yaay") /lose ("Please try again") audio --> Folder 10
- [x] wheels "success icon"

## Hardware

### Option stimuli
- [x] Individual neopixels / make some
- [x] YX5300 ~~& Logic Level Shifter (on breadboard) (don't seem to need the LLC actl, and it seems to make the YX TX --> ESP RX data not work)~~
- [x] Overlays
- [x] Wheels
- [x] Stepper mount

### User Controls
- [x] Scanning RGB Lightstrip - have the LEDs, havent connected

### Feedback
- [x] Vibration Motor & Relay
- [x] Feedback Lightstrip - have the LEDs, havent connected
- [x] Win ("Yaay") /lose ("Please try again") audio in Folder 10, Win as 001 and lose as 002
- [x] wheels "success icon"

### Power
- [x] Power Solution
- [x] 5V Power Solution

### Others
- [x] Masking tape labels on the wires
- [x] ~~solder one more F 3.5mm jack --> just use the screw one~~
- [ ] ~~Velcro for icons on the wheel --> just use double sided tape actually works~~

### In order of importance
#### High Effort High Impact
- [ ] Technical guide for AT staff on how to change visual (create strip of icons) and audio (Add MP3 files to SD Card) --> Eu Ler
- [ ] Gamemaster guide --> Eu Ler

#### Low Effort High Impact
- [ ] Win ("Yaay") /lose ("Please try again") audio --> some issues with playback
- [x] Audio files for drum and cat Vs dog --> Bingyu

#### Low Effort Low Impact
- [x] Prepare the strip for the wheel --> Bingyu
    - [ ] Add star icon to replace number icon on wheel

#### High Effort Low Impact
- NIL

### Eu Ler
- [x] Other lessons
    - In the menu just select between the numbers 1 to 8
    - [x] Folder 1: Piano Vs Flute
    - [ ] Folder 2: Drums Vs Flute --> hung on this once
    - [x] Folder 3: Dog Vs Cat
    - [ ] Folder 4-8: Custom
- [ ] Gamemaster guide
- [ ] Technical guide for AT staff on how to change visual (create strip of icons) and audio (Add MP3 files to SD Card)

### Buy/acquire
- [x] M3 Nuts

### Done
- [x] Box for each wheel --> Designed, Printing In Progress
    - [x] Cover up the visual icons that aren't being shown
- [x] The white button might hv some issues, check it
- [x] Give the wheel box and button the same number with the same text colour (just 2d print) --> 2D Printed
- [x] overall structure of presentation & split up who makes which part
- [x] For audio decide if they change lesson by changing SD Card or by pressing in gamemaster controls --> Change via software
- [x] For audio decide if we will tell them the volume and left/right channel difference can be done by hardware (e.g. relays and change vol command to MP3 module) --> Tell dont do
- [x] hv u verified if the names/numbers within the folders must also be unique for the whole card --> dont need be unique
- [x] come up w what additional stuff kiwi already has at his office that we want --> just the long stepper


## 3 Aug
### YR House
- [x] Rigid strip to mount feedback LED strip on --> 50cm ruler

### OTW Home
- [ ] timeline --> John
- [ ] Go through the 2nd visit to cpass again

### @ Home
- [x] Print strip & button labels --> John printed

## 4 Aug
- [ ] Box for electronics --> Assembly wip
- [ ] extend wires
- [ ] Our actual presentation
    - [ ] John
    - [ ] Bingyu
    - [ ] Yu Rou
- [ ] Consider: headphone jack come out of box