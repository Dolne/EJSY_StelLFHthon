#include <YX5300_ESP32.h> //SDCard MP3 player module, connects via UART

//***************************PIN DEFINITIONS***************************

//LED - Relay Ch1: 3.3V NO; input high --> close
//3.3V --> 18Ohm R --> LED --> Gnd
#define LED_PIN 26

//Vibration - Relay Ch1: 5V NO; input high --> close
#define VIBRATION_PIN 27

//Switches
#define SW1_PIN 22 //pull-up, connect other end of switch to ground

//Audio Module
//*make sure the RX on the YX5300 goes to the TX on the ESP32, and vice-versa
#define AUDIO_UART_RX 16 //Orange jumper
#define AUDIO_UART_TX 17 //White jumper

//***************************DECLARATIONS***************************
//Audio Module
YX5300_ESP32 audioModule; //Audio Module Object


void setup() {
  //***************************CONSTRUCTORS***************************
  //Audio Module
  audioModule = YX5300_ESP32(Serial2, AUDIO_UART_RX, AUDIO_UART_TX);
  // put your setup code here, to run once:

  //***************************GPIO SETTING***************************  
  //LED
  pinMode(LED_PIN, OUTPUT);
  //Vibration Motor
  pinMode(VIBRATION_PIN, OUTPUT);
  //Switch
  pinMode(SW1_PIN, INPUT_PULLUP);

  //***************************INITIALISATION***************************
  audioModule.setVolume(30);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (digitalRead(SW1_PIN) == 1) {
    audioModule.playTrackInFolder(1, 1);
    digitalWrite(LED_PIN, 1);
    digitalWrite(VIBRATION_PIN, 0);

    while (digitalRead(SW1_PIN) == 1) { //stops repeated setting of GPIO & MP3
      delay(10);
    }
  }

  else {
    audioModule.playTrackInFolder(6, 2);
    digitalWrite(LED_PIN, 0);
    digitalWrite(VIBRATION_PIN, 1);

    while (digitalRead(SW1_PIN) == 0) { //stops repeated setting of GPIO & MP3
      delay(10);
    }
  }
}
