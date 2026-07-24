# EJSY\_StelLFHthon Finals Readme

### Check before beginning
* Is the SD card in the MP3 module

### Terminology
* Icon = the 10 faces of a wheel
  * The first of which is blank
  * The second of which is the corresponding wheel's number
  * Images = all the faces from the third to the last one

### MP3 Player module
microsd card containing the audio
* https://github.com/bluejunimo/YX5300_ESP32
1. First, format your microSD card as FAT16 or FAT32.
2. Then, create however many folders you want starting with the label "01" and incrementing with each new folder. Think of folders as different playlists.
3. Your tracks need to have a 3-digit prefix at the start. They each must be unique (not just within, but between folders too), but naming them sequentially is the easiest way. Anything after the prefix is ignored by the module.
* **The MP3 module needs ~2s from the moment it receives power till the moment it's actually ready to send/receive UART data, so your code cannot immediately on setup ask to play a file**

#### Logic level convertrer
* At least channel 1 works from LV --> HV

#### Masking tape
* The connectors are usually bunched up with masking tape
* The pins each wire corresponds to is either written on  the female/male end of the wire, or it matches the order of the connections written on the masking tape (e.g. Gnd, 3.3V, Pin 5 next to Red, Green, Blue colour wire means Red=Gnd, Green=3.3V, Blue=Pin5)

### Uploading code to the ESP

* Select "ESP32 Dev Module", not something else
* If you're using the one from Lionsforge (with the expansion board), download the drivers for the CP2102 from e.g. https://www.silabs.com/software-and-tools/usb-to-uart-bridge-vcp-drivers?tab=downloads
* ESP Board library V3.3.5 is compatible, V3.3.10 isnt

### Connections to the ESP

Reference https://randomnerdtutorials.com/esp32-pinout-reference-gpios/

Unassigned pins as letters
Total 25 pins

* Gamemaster Controls Subsystem
  * I2C (for 20x4 LCD)
    * SDA (Serial Data): **21**
    * SCL (Serial Clock): **22**
  * 3 buttons
    * **MCP23017 pins 0, 1 and 2**
* User Controls Subsystem
  * 4 buttons
    * **5**
    * **18**
    * **19**
    * **23**
* Option Stimuli \& Feedback Subsystem
  * Visual Stimuli
    * Stepper (Step, Dir, Homing LightGate)
      * Stepper A: **26, 12, 34**
      * Stepper B: **27, 13, 35**
      * Stepper C: **32, 14, 36** (pin 34 is input only)
      * Stepper D: **33, 15, 39** (pin 35 is input only)
  * MP3 Module (UART)
    * ESP TX --> Module RX: **17**
    * ESP RX <-- Module TX: **16**
  * Vibration Motor Relay: **2**
  * Feedback RGB Lightstrip: **25**
  * Scanning RGB Lightstrip: **4**

~~For testing, gamemaster controls is --> dont need, the MCP23017 expander works liao~~
* ~~Down: 4~~
* ~~Select: 17~~
* ~~Up: 25~~

All pins

0
~~1,~~ (Serial Monitor)

**2,**

~~3,~~ (Serial Monitor)

**4, 5,**

~~6, 7, 8, 9, 10, 11,~~ (internal SPI)

**12, 13, 14, 15,** **16, 17**, **18, 19,**

**21, 22,** **23,**

**25,** **26, 27,**

**32, 33,**

**34, 35,** **36, 39**

### The stuff from LF

* The ESP32 lowk don't work, can connect but can't seem to upload code

### Settings for the steppers so they dont draw too much current and cause the wires to melt
- setMaxSpeed(3000); 
- setAcceleration(200);

## ToDo
### Outstanding features software

#### Software ↔︎ Hardware
- [x] YX5300 (Audio Player)
- [x] validate audio ([validate_audio_alt](./validate_audio_alt/validate_audio_alt.ino)) and create audio files, then uncomment audio code
- [ ] stepper homing sequence (light gates)
- [x] bring steppers to 0 at end of game or before turning off
- [ ] periodically refresh entire screen

#### Gamemaster
- [ ] Show options for tactile on gamemaster screen
- [ ] Check/validate options (eg no diffs selected)

#### User Controls
- [x] Individually addressable RGB for scanning

#### Feedback
- [x] Individually addressable RGB for feedback
- [x] Vibration Motor
- [x] Round / Score messages

### Outstanding features hardware
#### Option stimuli
- [x] Individual neopixels / make some
- [x] YX5300 ~~& Logic Level Shifter (on breadboard) (don't seem to need the LLC actl, and it seems to make the YX TX --> ESP RX data not work)~~
- [ ] LightGate - might forgo for now since no time
- [x] Overlays
- [x] Wheels
- [x] Stepper mount

#### User Controls
- [x] Scanning RGB Lightstrip - have the LEDs, havent connected

#### Feedback
- [x] Vibration Motor & Relay
- [x] Feedback Lightstrip - have the LEDs, havent connected

#### Power
- [x] Power Solution
- [x] 5V Power Solution

#### Others
- [x] Masking tape labels on the wires
* The white button might hv some issues, check it
* Need solder one more F 3.5mm jack