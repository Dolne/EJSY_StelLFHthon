#include <Arduino.h>
#include <MD_YX5300.h>

/**
 * use the MD_YX5300 library to interface with the audio player asynchronously
 * 
 * this aims to validate the following functionality:
 *  1. it is able to play audio
 *  2. it sends the STS_FILE_END status code when it finishes playing audio
 *  3. it sends the STS_ERR_FILE status code if there is an issue with playing audio (e.g. file not found)
 * 
 * to validate 3, change the folder/file played by mp3.playSpecific
 */

MD_YX5300 mp3(Serial2);

double startTime = 0;

void setup() {
    Serial2.begin(MD_YX5300::SERIAL_BPS, SERIAL_8N1, 16, 17);
    Serial.begin(115200);
    mp3.begin();
    mp3.setSynchronous(false);
    //Serial.print("Maximum volume: ");
    //Serial.println(mp3.volumeMax());
    delay(2000);
    mp3.volume(10);
    delay(2000);
    startTime = millis();
    Serial.println("Folder 10, file 152");
    mp3.playSpecific(10, 152); //folder, file
    delay(5000);
    Serial.println("Folder 10, file 151");
    mp3.playSpecific(10, 151); //folder, file
}

void loop() {
    if (((millis() - startTime) > 20000) && ((millis() - startTime) < 20000)) { //20s
        Serial.println("Folder 10, file 152");
        mp3.playSpecific(10, 152); //folder, file
        delay(5000);
    }

    else if ((millis() - startTime) > 12000) { //12s
        Serial.println("Folder 10, file 151");
        mp3.playSpecific(10, 151); //folder, file
        delay(9000);
    }
    

    if (mp3.check()) {
        switch(mp3.getStsCode()) {
            case MD_YX5300::STS_ACK_OK:
                // last command acknolwedged
                Serial.println("STS_ACK_OK");
                break;
            case MD_YX5300::STS_FILE_END:
                // file being played has ended
                Serial.println("STS_FILE_END");
                break;
            case MD_YX5300::STS_ERR_FILE:
                // error playing file
                Serial.println("STS_ERR_FILE");
                break;
            case MD_YX5300::STS_INIT:
                // initialisaion complete
                Serial.println("STS_INIT");
                break;
        }
    }
}