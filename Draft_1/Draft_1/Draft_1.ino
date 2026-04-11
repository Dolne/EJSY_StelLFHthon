//***************************LIBRARIES***************************
//MP3 Player
#include <YX5300_ESP32.h> //SDCard MP3 player module, connects via UART

//WiFi & MQTT libraries
#include <PubSubClient.h>
#include <WiFi.h>

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

//***************************MQTT TOPIC NAMES***************************
//MQTT topic names for lift adapter
const char* liftAdapter_nextMessageToFleet_topic = "lift_adapter_transceiver/next_message_to_fleet";
const char* liftAdapter_messageSentCfm_topic = "lift_adapter_transceiver/message_sent";
const char* liftAdapter_messageFromFleet_topic = "lift_adapter_transceiver/message_from_fleet";
const char* liftAdapter_tellDoorState_topic = "lift_adapter_transceiver/door_state";
//MQTT topic names for fleet adapter 
const char* fleetAdapter_nextMessageToLift_topic = "fleet_adapter_transceiver/next_message_to_lift";
const char* fleetAdapter_messageSentCfm_topic = "fleet_adapter_transceiver/message_sent";
const char* fleetAdapter_messageFromLift_topic = "fleet_adapter_transceiver/message_from_lift";
const char* fleetAdapter_tellDoorState_topic = "fleet_adapter_transceiver/door_state";
//MQTT topic names for lift sim
const char* liftSim_doorState_topic = "lift_sim/door_state"; //Subscribed to by both
const char* liftSim_currentLevel_topic = "lift_sim/curr_level"; //Subscrived to by fleet adapter

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
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (digitalRead(SW1_PIN) == 1) {
    audioModule.playTrackInFolder(1, 1);
    digitalWrite(LED_PIN, 1);
    digitalWrite(VIBRATION_PIN, 0);
    debugMessage("Sw On");

    while (digitalRead(SW1_PIN) == 1) { //stops repeated setting of GPIO & MP3
      delay(10);
    }
  }

  else {
    audioModule.playTrackInFolder(6, 2);
    digitalWrite(LED_PIN, 0);
    digitalWrite(VIBRATION_PIN, 1);
    debugMessage("Sw Off");

    while (digitalRead(SW1_PIN) == 0) { //stops repeated setting of GPIO & MP3
      delay(10);
    }
  }
}

void debugMessage(char* message){ //Send a debugging message out, for now via USB Serial only
  Serial.println(message);
}

