// This example code is in the Public Domain (or CC0 licensed, at your option.)
// By Evandro Copercini - 2018
//
// This example creates a bridge between Serial and Classical Bluetooth (SPP)
// and also demonstrate that SerialBT have the same functionalities of a normal Serial
// Note: Pairing is authenticated automatically by this device

#include <Arduino.h>
#include "BluetoothSerial.h"

String BT_NAME = "ESP32-BtSerial"; //BT Device name for BT Serial

// Check if Bluetooth is available
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

// Check Serial Port Profile
#if !defined(CONFIG_BT_SPP_ENABLED)
#error Serial Port Profile for Bluetooth is not available or not enabled. It is only available for the ESP32 chip.
#endif

BluetoothSerial BtSerial;

void setup() {
  Serial.begin(115200);
  BtSerial.begin(BT_NAME);  //Bluetooth device name
  BtSerial.deleteAllBondedDevices(); // Uncomment this to delete paired devices; Must be called after begin
  Serial.printf("The device with name \"%s\" is started.\nNow you can pair it with Bluetooth!\n", BT_NAME.c_str());
}

void loop() {
  for (int i=0; i<10000; i++){
    delay(500);
    btAndSerialPrint(String(i));
    serialCopy();
  }
}


void btAndSerialPrint(String inputMessage) {
  char messageCharArray[inputMessage.length()];
  strcpy(messageCharArray, inputMessage.c_str());
  BtSerial.println(messageCharArray);
  Serial.println(messageCharArray);
}

String serialCopy() { //Rebroadcasts & Returns serial message received
  while (Serial.available() > 0) { //Serial available returns the number of bytes available for reading from the serial port
    BtSerial.write(Serial.read()); //Goes through one byte at a time
  }
  while (BtSerial.available() > 0) {
    Serial.write(BtSerial.read());
  }

  return String("");
}