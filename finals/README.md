# finals

TODO: more info on how to run code, simulation etc
TODO: this should also explain stuff about the code

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

## User controls

### User buttons

The user buttons are connected via a 3.5mm mono audio jack, and assumed to be "normally open".

On setup, the code assumes that all buttons are not pressed, and will read the value of the pins to use as the inactive value.

If the a button is changed to one with a different value when active, the debug menu can be used to reset the active value for all the user buttons. Press the BOOT button on the ESP to open the debug menu, and navigate down to the "Reset user buttons" options. Ensure that none of the user buttons are pressed when resetting. The debug menu should then show that all the user buttons are not pressed.

| Button | Pin |
| ------ | --- |
| 1      | 5   |
| 2      | 18  |
| 3      | 19  |
| 3      | 23  |

## Stimuli/feedback

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

### LED light strips

There are 2 NeoPixel (WS2812?) LED light strips, with one used for feedback (answer right/wrong) at the end of rounds, and the other for scanning — indicating the current slot for selection.

The feedback light strip has 30 LEDs, while the scanning light strip consists of 4 separate NeoPixels wired together.

The light strips are controlled using the [Adafruit NeoPixel library](https://adafruit.github.io/Adafruit_NeoPixel/html/class_adafruit___neo_pixel.html).

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