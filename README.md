# EJSY\_StelLFHthon Finals Readme



### Uploading code to the ESP

* Select "ESP32 Dev Module", not something else
* If you're using the one from Lionsforge (with the expansion board), download the drivers for the CP2102 from e.g. https://www.silabs.com/software-and-tools/usb-to-uart-bridge-vcp-drivers?tab=downloads

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
- [ ] validate audio ([validate_audio_alt](./validate_audio_alt/validate_audio_alt.ino)) and create audio files, then uncomment audio code
- [ ] stepper homing sequence (light gates)
- [ ] bring steppers to 0 at end of game or before turning off? Yes
- [ ] periodically refresh entire screen

#### Gamemaster
- [ ] Show options for tactile on gamemaster screen
- [ ] Check/validate options (eg no diffs selected)

#### User Controls
- [ ] Individually addressable RGB for scanning

#### Feedback
- [x] Individually addressable RGB for feedback
- [x] Vibration Motor
- [x] Round / Score messages

### Outstanding features hardware
#### Option stimuli
- [x] Individual neopixels / make some
- [ ] YX5300 & Logic Level Shifter (on breadboard)
- [ ] LightGate
- [x] Overlays
- [x] Wheels
- [x] Stepper mount

#### User Controls
- [ ] Scanning RGB Lightstrip - have the LEDs, havent connected

#### Feedback
- [ ] Vibration Motor & Relay
- [ ] Feedback Lightstrip - have the LEDs, havent connected

#### Power
- [x] Power Solution
- [x] 5V Power Solution

#### Others
- [x] Masking tape labels on the wires
