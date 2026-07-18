#include <YX5300_ESP32.h> //SDCard MP3 player module, connects via UART

//PIN DEFINITIONS

//Audio module
// *make sure the RX on the YX5300 goes to the TX on the ESP32, and vice-versa
#define AUDIO_UART_RX 16 //Orange jumper
#define AUDIO_UART_TX 17 //White jumper

//DECLARATIONS
YX5300_ESP32 audioModule; //Audio Module Object

void setup() {
  // initialize connection with the module
  audioModule = YX5300_ESP32(Serial2, AUDIO_UART_RX, AUDIO_UART_TX);
  
  // Shows whats being sent to and from the device, with helpful errors
  Serial.begin(115200);
  audioModule.enableDebugging();

  audioModule.setVolume(30);

  
  
  // Begins playing from 001.mp3/wav and continuously loops through all the songs 
  // on the SD card ONLY if there is no track already playing or paused.
  // mp3.resume();
  
  // other ways to begin playing tracks (see documentation for functionality)
  // mp3.playTrack(1);
  // mp3.playTrackInLoop(1);
  // mp3.playFolderInLoop(1);
  audioModule.playTrackInFolder(3, 24);

}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(audioModule.queryDeviceState());
  delay(100);

}


