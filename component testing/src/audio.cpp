#include <Arduino.h>
#include <YX5300_ESP32.h> 

#define AUDIO_UART_RX 16 //Orange jumper
#define AUDIO_UART_TX 17 //White jumper

YX5300_ESP32 audioModule; //Audio Module Object

void playAudio(int track, int folder) { //Play audio from the Audio Module
  audioModule.playTrackInFolder(track, folder);
//   debugMessage(String("MP3 Playing: Track ") + track + String(" within folder ") + folder);
}

void setup(){
    delay(3000);
    audioModule = YX5300_ESP32(Serial2, AUDIO_UART_RX, AUDIO_UART_TX);
    //audioModule.reset();
    audioModule.setVolume(15);
    delay(1000);
    audioModule.playTrackInFolder(1,1);
}

void loop(){

    // audioModule.playTrackInFolder(6,2);
}