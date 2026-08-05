# finals

This is the implementation of our game for the hackathon finals.

## ESP32

The source files for the ESP32 are contained in the [main](./main/) folder which is meant to be opened in the Arduino IDE.

### Uploading code

- Select "ESP32 Dev Module", not something else
- If you're using the one from Lionsforge (with the expansion board), download the drivers for the CP2102 from e.g. https://www.silabs.com/software-and-tools/usb-to-uart-bridge-vcp-drivers?tab=downloads
- ESP Board library V3.3.5 is compatible, V3.3.10 has issues

## Simulation

The source files are also used for simulating the project in [Wokwi](https://wokwi.com), so all code must be written to run on both the actual hardware and also the simulation. In particular, note that the [main/main.ino](./main/main.ino) file will be run as a C++ file in the simulation.

The `SIMULATION` constant is set to `1` when running in the simulator, and `0` for the actual hardware. This can be used for example to define different pins for the simulation and hardware or omit certain things which cannot be simulated.

The [simulation](./simulation/) folder is a [PlatformIO](https://platformio.org) project which needs to be opened as a VS Code workspace to work properly.

See [simulation/README.md](./simulation/README.md) for more info.

## Gamemaster controls

The gamemaster configures the game options and controls the game using a 20x4 character LCD display with 3 buttons.

The gamemaster can control settings like the number of rounds in the game as well as the [input mode](#user-input), and control whether certain stimuli are active and how the odd one out should differ from the others. The gamemaster controls are also used to go to the next round of the game or start a new game.

The screen displays a menu, which is navigated using the 3 buttons — up, action, and down. The up and down buttons control which row is selected, and will cause the screen to scroll if necessary.

There are 3 different kinds of menu rows:

1.  Info rows which just display some text.

    Pressing the action button does nothing when these rows are selected.

2.  Action rows which are like a button to do something.

    Pressing the action button will do something as described on the row.

3.  Option rows which have a label and a value.

    Pressing the action button will cycle through the possible values for that option. Each option row is linked to a specific game option.

If there is something interactive, either an action row or value for an option row, it will blink on the screen to indicate that the action button can be pressed to do something.

### Gamemaster LCD

20x4 LCD character display over I2C using the [LiquidCrystal_I2C library](https://registry.platformio.org/libraries/marcoschwartz/LiquidCrystal_I2C). The display is driven by a HD44780 controller over I2C on address `0x27`.

| LCD I2C | Pin |
| ------- | --- |
| SDA     | 21  |
| SCL     | 22  |

### Gamemaster buttons

The gamemaster buttons are "normally closed" and connected to the MCP23017 I2C GPIO expander.

On setup, the code assumes that all buttons are not pressed, and will read the value of the pins to use as the inactive value. To reset the active values for the gamemaster buttons, restart/reset the ESP.

| Button | Pin |
| ------ | --- |
| Up     | A0  |
| Action | A1  |
| Down   | A2  |

## User input

The game has 2 modes for input — scanning and select.

The scanning mode works using a single input. The game will scan through each option one by one, allowing the user to select the highlighted option by pressing their input.

The select mode works using 4 separate inputs. Each input will correspond to a specific slot, which will be selected by pressing the corresponsing input.

All the user inputs are connected using a 3.5mm mono audio jack, which is a common interface for assistive switches, allowing the user to easily swap to using their own input methods.

### User buttons

The user buttons are connected via a 3.5mm mono audio jack, and assumed to be "normally open".

On setup, the code assumes that all buttons are not pressed, and will read the value of the pins to use as the inactive value.

If the a button is changed to one with a different value when active, the debug menu can be used to reset the active value for all the user buttons. Press the BOOT button on the ESP to open the debug menu, and navigate down to the "Reset user buttons" options. Ensure that none of the user buttons are pressed when resetting. The debug menu should then show that all the user buttons are not pressed.

| Button | Pin |
| ------ | --- |
| 1      | 5   |
| 2      | 18  |
| 3      | 19  |
| 4      | 23  |

## Stimuli/feedback

In each round, the game will present 3 or 4 options to the user (based on what the gamemaster chooses). Of which, one of the options will be different from the others. These options can be visual, audio, or tactile, or even with multiple senses at once depending on what the gamemaster chooses.

The visual stimuli are shapes on slot machine style wheels which are controlled by stepper motors. There are 3 different features of the shapes which can differ — the shape, colour, and size.

The audio stimuli are audio files played from an SD card and have 4 features — the pitch, loudness, timbre, and panning (L/R).

For the visual and audio stimuli, each option is represented by an index. Each feature has 2 possible values, and these values are used to generate the index. The game will first random generate an index to use as the odd one out, then based on the game options, toggle certain features to generate the index used for all the other options.

The tactile stimuli are physically passed to the user by the gamemaster. The game does not know about the features of the tactile stimuli, and will only tell the gamemaster which is the odd one out option.

At the end of each round, the game will provide feedback to the user on whether their answer is correct or wrong using the speakers, LED strips, and a vibration motor. It will also display the score to the gamemaster with the option to allow the user to retry if their answer is wrong.

### Steppers

The steppers control the visual stimuli of the game, each spinning a wheel with 10 faces

- Icon = the 10 faces of a wheel
  - The first of which is blank (index 0)
  - The second of which is the corresponding wheel's number (index 1)
  - Images = all the faces from the third to the last one (index 2–9)

The [AccelStepper library](https://www.airspayce.com/mikem/arduino/AccelStepper/classAccelStepper.html) is used to control multiple steppers at once with smooth acceleration and deccelration.

The acceleration and max speed of the steppers must be limited in software to prevent drawing too much current.

1/8 microstepping used with the TMC2160-OC stepper drivers for 1600 (micro)steps per rotation.

| Stepper | STEP | DIR | Homing LightGate |
| ------- | ---- | --- | ---------------- |
| A       | 26   | 12  | 34 (input only)  |
| B       | 27   | 13  | 35 (input only)  |
| C       | 32   | 14  | 36               |
| D       | 33   | 15  | 39               |

### Sequence for Visual

For our current set up

- Shape
  - Shape A is a circle
  - Shape B is a square
- Colour
  - Colour A is Blue
  - Colour B is Green
- Size
  - Size A is small
  - Size B is large

| Index | Shape  | Colour | Size  |
| ----: | ------ | ------ | ----- |
|     0 | blank  |        |       |
|     1 | star   |        |       |
|     2 | circle | blue   | small |
|     3 | circle | blue   | large |
|     4 | circle | green  | small |
|     5 | circle | green  | large |
|     6 | square | blue   | small |
|     7 | square | blue   | large |
|     8 | square | green  | small |
|     9 | square | green  | large |

This can be customized by replacing the icons on the wheel, provided it follows a similar ordering based on the shape, colour, and size.

### MP3 Module

Audio is played using a YX5300 serial MP3 player from an inserted microSD card.

1. First, format your microSD card as FAT16 or FAT32.
2. Then, create however many folders you want starting with the label "01" and incrementing with each new folder. Think of folders as different playlists.
3. Your tracks need to have a 3-digit prefix at the start. They each must be unique (not just within, but between folders too), but naming them sequentially is the easiest way. Anything after the prefix is ignored by the module.

> [!IMPORTANT]
> The MP3 module needs ~2s from the moment it receives power till the moment it's actually ready to send/receive UART data, so the code cannot immediately on setup ask to play a file.

The [MD_YX5300 library](https://majicdesigns.github.io/MD_YX5300/class_m_d___y_x5300.html) is used to interface with the YX5300 module in a asynchronous/non-blocking manner, which is crucial for the steppers to work properly.

The code relies on receiving the unsolicted FILE_END status response from the YX5300 to know when the audio has finished playing. This is not supported by the YX5300_ESP32 library which is commonly used in examples online.

| YX5300 | Pin      |
| ------ | -------- |
| RX     | 16 (TX2) |
| TX     | 17 (RX2) |

### Sequence for Audio

By default, there are 3 folders of different audio the gamemaster can choose from.

Additionally, the gamemaster can add up to 5 custom folders numbered 04 to 08 in the SD card. The audio must follow the same order shown below — based on the panning (L/R), timbre, pitch, and loudness.

#### 01. Piano vs Flute

| Filename | LR    | Timbre | Pitch | Loudness |
| -------: | ----- | ------ | ----- | -------- |
|      001 | left  | piano  | low   | soft     |
|      002 | left  | piano  | low   | loud     |
|      003 | left  | piano  | high  | soft     |
|      004 | left  | piano  | high  | loud     |
|      005 | left  | flute  | low   | soft     |
|      006 | left  | flute  | low   | loud     |
|      007 | left  | flute  | high  | soft     |
|      008 | left  | flute  | high  | loud     |
|      009 | right | piano  | low   | soft     |
|      010 | right | piano  | low   | loud     |
|      011 | right | piano  | high  | soft     |
|      012 | right | piano  | high  | loud     |
|      013 | right | flute  | low   | soft     |
|      014 | right | flute  | low   | loud     |
|      015 | right | flute  | high  | soft     |
|      016 | right | flute  | high  | loud     |

#### 02. Piano vs Drums

| Filename | LR    | Timbre | Pitch | Loudness |
| -------: | ----- | ------ | ----- | -------- |
|      001 | left  | piano  | low   | soft     |
|      002 | left  | piano  | low   | loud     |
|      003 | left  | piano  | high  | soft     |
|      004 | left  | piano  | high  | loud     |
|      005 | left  | drum   | low   | soft     |
|      006 | left  | drum   | low   | loud     |
|      007 | left  | drum   | high  | soft     |
|      008 | left  | drum   | high  | loud     |
|      009 | right | piano  | low   | soft     |
|      010 | right | piano  | low   | loud     |
|      011 | right | piano  | high  | soft     |
|      012 | right | piano  | high  | loud     |
|      013 | right | drum   | low   | soft     |
|      014 | right | drum   | low   | loud     |
|      015 | right | drum   | high  | soft     |
|      016 | right | drum   | high  | loud     |

#### 03. Dog vs Cat

| Filename | LR    | Timbre | Pitch | Loudness |
| -------: | ----- | ------ | ----- | -------- |
|      001 | left  | dog    | low   | soft     |
|      002 | left  | dog    | low   | loud     |
|      003 | left  | dog    | high  | soft     |
|      004 | left  | dog    | high  | loud     |
|      005 | left  | cat    | low   | soft     |
|      006 | left  | cat    | low   | loud     |
|      007 | left  | cat    | high  | soft     |
|      008 | left  | cat    | high  | loud     |
|      009 | right | dog    | low   | soft     |
|      010 | right | dog    | low   | loud     |
|      011 | right | dog    | high  | soft     |
|      012 | right | dog    | high  | loud     |
|      013 | right | cat    | low   | soft     |
|      014 | right | cat    | low   | loud     |
|      015 | right | cat    | high  | soft     |
|      016 | right | cat    | high  | loud     |

#### System Audio

There is also some system audio in folder 10. Currently it contains the audio to play as part of the feedback after the user inputs an answer.

| Filename | Audio          |
| -------: | -------------- |
|       01 | Success        |
|       02 | Fail/try again |

### LED light strips

There are 2 NeoPixel (WS2812?) LED light strips, with one used for feedback (answer right/wrong) at the end of rounds, and the other for scanning — indicating the current slot for selection.

The feedback light strip has 30 LEDs, while the scanning light strip consists of 4 separate NeoPixels wired together.

The light strips are controlled using the [FastLED library](https://github.com/FastLED/FastLED).

| Light strip | Pin |
| ----------- | --- |
| Feedback    | 25  |
| Scanning    | 4   |

### Vibration motor

The vibration motor is enabled/disabled via a relay.

| Relay           | Pin |
| --------------- | --- |
| Vibration motor | 2   |

## Connections

### Labelling

- The connectors are usually bunched up with masking tape
- The pins that each wire corresponds to is either written on the female/male end of the wire, or it matches the order of the connections written on the masking tape
  (e.g. Gnd, 3.3V, Pin 5 next to Red, Green, Blue colour wire means Red=Gnd, Green=3.3V, Blue=Pin5)

### ESP32 Pin Overview

Reference https://randomnerdtutorials.com/esp32-pinout-reference-gpios/

| GPIO             | Connection                |
| ---------------- | ------------------------- |
| 0 (BOOT)         | built-in BOOT button      |
| 1 (TX0)          |                           |
| 2                |                           |
| 3 (RX0)          |                           |
| 4                | scanning light strip      |
| 5                | user button 1             |
| 6–11 (SPI flash) | ❌                        |
| 12               | stepper A DIR             |
| 13               | stepper B DIR             |
| 14               | stepper C DIR             |
| 15               | stepper D DIR             |
| 16 (RX2)         | MP3 module                |
| 17 (TX2)         | MP3 module                |
| 18               | user button 2             |
| 19               | user button 3             |
| 21 (I2C SDA)     | gamemaster LCD & MCP23017 |
| 22 (I2C SCL)     | gamemaster LCD & MCP23017 |
| 23               | user button 4             |
| 25               | feedback light strip      |
| 26               | stepper A STEP            |
| 27               | stepper B STEP            |
| 32               | stepper C STEP            |
| 33               | stepper D STEP            |
| 34 (input only)  | stepper A lightgate       |
| 35 (input only)  | stepper B lightgate       |
| 36 (input only)  | stepper C lightgate       |
| 39 (input only)  | stepper D lightgate       |

### MCP23017

The MCP23017 I2C GPIO expander is used to give some extra pins, and is connected to the I2C SDA and SCL pins on the ESP32 along the gamemaster LCD.

The [Adafruit MCP23017 library](https://adafruit.github.io/Adafruit-MCP23017-Arduino-Library/html/class_adafruit___m_c_p23_x17.html) is used to interface with the module.

| MCP23017 | Pin |
| -------- | --- |
| SDA      | 21  |
| SCL      | 22  |

#### Pin overview

| GPIO  | Connection               |
| ----- | ------------------------ |
| A0    | Gamemaster up button     |
| A1    | Gamemaster action button |
| A2    | Gamemaster down button   |
| A3–A7 |                          |
| B0–B7 |                          |
