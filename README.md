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

    * **A**
    * **B**
    * **C**
* User Controls Subsystem

  * 4 buttons

    * **5**
    * **18**
    * **19**
    * **23**
* Option Stimuli \& Feedback Subsystem

  * Visual Stimuli

    * Stepper (Step, Dir, Homing LightGate)

      * Stepper 1: **26, 12, 34**
      * Stepper 2: **27, (13), 35**
      * Stepper 3: **32, 14, 36** (pin 34 is input only)
      * Stepper 4: **33, 15, 39** (pin 35 is input only)
  * MP3 Module (UART)

    * ESP TX --> Module RX: **17**
    * ESP RX <-- Module TX: **16**
  * Vibration Motor Relay: **4**
  * RGB Lightstrip: **25**



All pins

0
~~1,~~ (Serial Monitor)

2,

~~3,~~ (Serial Monitor)

**4, 5,**

~~6, 7, 8, 9, 10, 11,~~ (internal SPI)

**12, 13, 14, 15,** **16, 17**, **18, 19,**

**21, 22,** **23,**

**25,** **26, 27,**

**32, 33,**

**34, 35,** **36, 39**

### 

### The stuff from LF

* The ESP32 lowk don't work, can connect but can't seem to upload code

