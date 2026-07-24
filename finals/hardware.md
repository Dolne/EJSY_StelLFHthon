# Hardware Overview

## ESP32

### Uploading code

- Select "ESP32 Dev Module", not something else
- If you're using the one from Lionsforge (with the expansion board), download the drivers for the CP2102 from e.g. https://www.silabs.com/software-and-tools/usb-to-uart-bridge-vcp-drivers?tab=downloads
- ESP Board library V3.3.5 is compatible, V3.3.10 has issues

## Gamemaster controls

### Gamemaster LCD

20x4 LCD character display over I2C.

| LCD I2C | Pin |
| ------- | --- |
| SDA     | 21  |
| SCL     | 22  |

### Gamemaster buttons

The gamemaster buttons are "normally closed" and connected to the MCP23017 I2C GPIO expander.

| Button | Pin |
| ------ | --- |
| Up     | A0  |
| Action | A1  |
| Down   | A2  |

## User controls

### User buttons

The user buttons are connected via a 3.5mm mono audio jack, and assumed to be "normally open".

| Button | Pin |
| ------ | --- |
| 1      | 5   |
| 2      | 18  |
| 3      | 19  |
| 3      | 23  |

## Simuli/feedback

### Steppers

The acceleration and max speed of the steppers must be limited in software to prevent drawing too much current.

The steppers control the visual stimuli of the game, each spinning a wheel with 10 faces
- Icon = the 10 faces of a wheel
  - The first of which is blank (index 0)
  - The second of which is the corresponding wheel's number (index 1)
  - Images = all the faces from the third to the last one (index 2–9)

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
> The MP3 module needs ~2s from the moment it receives power till the moment it's actually ready to send/receive UART data, so your code cannot immediately on setup ask to play a file

| YX5300 | Pin      |
| ------ | -------- |
| RX     | 16 (TX2) |
| TX     | 17 (RX2) |

### LED light strips

There are 2 NeoPixel (WS2812?) LED light strips, with one used for feedback (answer right/wrong) at the end of rounds, and the other for scanning — indicating the current slot for selection.

The feedback light strip has 30 LEDs, while the scanning light strip consists of 4 separate NeoPixels wired together.

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
| 0                |                           |
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

The MCP23017 I2C GPIO expander is used to give some extra pins.

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
