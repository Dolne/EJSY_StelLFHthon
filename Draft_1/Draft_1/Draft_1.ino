//***************************LIBRARIES***************************
//MP3 Player
#include <YX5300_ESP32.h> //SDCard MP3 player module, connects via UART

//WiFi & MQTT libraries
#include <PubSubClient.h>
#include <WiFi.h>

//***************************SETTINGS/GPIO ASSIGNMENT***************************

//LED - Relay Ch1: 3.3V NO; input high --> close
//3.3V --> 18Ohm R --> LED --> Gnd
#define LED_PIN 26

//Vibration - Relay Ch1: 5V NO; input high --> close
#define VIBRATION_PIN 27

//Switches
#define SW1_PIN 22 //pull-up, connect other end of switch to ground
#define SW2_PIN 4 //pull-up, connect other end of switch to ground
#define SW3_PIN 12 //pull-up, connect other end of switch to ground
#define SW4_PIN 13 //pull-up, connect other end of switch to ground //This is slower for some reason

//Audio Module
//*make sure the RX on the YX5300 goes to the TX on the ESP32, and vice-versa
#define AUDIO_UART_RX 16 //Orange jumper
#define AUDIO_UART_TX 17 //White jumper

//WiFI settings
const char *hostname = "hthon_ESP";
const char *ssid = "TP-Link_8E20"; //WiFi SSID
const char *password = "P@ssw0rd123"; //WiFi password //Admin panel pw P@sw0rd123

//MQTT settings
const char *clientID = "hthon_ESP";
const char *mqttServerIP = "192.168.1.101"; //MQTT server IP address e.g. 192.168.18.3

//MQTT TOPIC NAMES
//Send
const char* debug_topic = "debugMessage";
const char* error_topic = "errorMessage";
const char* display_topic = "display";
//Receive (Subscribe)
const char* settings_topic = "settings";
const char* espCommands_topic = "espCommands";
//TOADD: Could change the above to an array of subcriptions

//***************************DECLARATIONS***************************
//Audio Module
YX5300_ESP32 audioModule; //Audio Module Object

//WiFi & MQTT objects
WiFiClient WiFiC; //WiFiClient derived from ESP32's WiFi connection
static PubSubClient MQTTclient(WiFiC); //Initial partial configuration of PubSubClient

//For multi-core utilisation
TaskHandle_t MQTTloopTask = NULL;
TaskHandle_t mainTask = NULL;

//***************************GLOBAL VAIRABLES***************************
int buttonFlags[4] = {0, 0, 0, 0}; //1 indicates button got pressed, -1 indicates released

//******************************************************CODE******************************************************

//***************************DEBUG & ERROR***************************
void debugMessage(char* message) { //Send a debugging message out via MQTT & USB Serial
  Serial.println(message);
  MQTTpublishWithSerial(debug_topic, message);
}

void debugMessage(const char* message) { //Send a debugging message out via MQTT & USB Serial
  Serial.println(message);
  MQTTpublishWithSerial(debug_topic, message);
}

void debugMessage(String message) { //Send a debugging message out via MQTT & USB Serial
  Serial.println(message.c_str());
  MQTTpublishWithSerial(debug_topic, message.c_str());
}

void errorMessage(char* message) { //Send an error message out via MQTT & USB Serial
  Serial.println(message);
  MQTTpublishWithSerial(error_topic, message);
}

void errorMessage(const char* message) { //Send an error message out via MQTT & USB Serial
  Serial.println(message);
  MQTTpublishWithSerial(error_topic, message);
}

//***************************HARDWARE FUNCTIONS***************************
//These exist in place of directly calling the relevant functions to enable debugging & error messages
void ledStrip(bool onOff) { //Turn the LED Strip on or off
  digitalWrite(LED_PIN, onOff);
    if (onOff) {
      digitalWrite(LED_PIN, 1);
      debugMessage(String("LED Strip on"));
    }
    if (onOff == false) {
      digitalWrite(LED_PIN, 0);
      debugMessage(String("LED Strip off"));
    }
}

void vibrationMotor(bool onOff) { //Turn the vibration motor on or off, true/1=on
  digitalWrite(VIBRATION_PIN, onOff);
  if (onOff) {
    digitalWrite(VIBRATION_PIN, 1);
    debugMessage(String("Vibration motor on"));
  }
  if (onOff == false) {
    digitalWrite(VIBRATION_PIN, 0);
    debugMessage(String("Vibration motor off"));
  }
}

void playAudio(int track, int folder) { //Play audio from the Audio Module
  audioModule.playTrackInFolder(track, folder);
  debugMessage(String("MP3 Playing: Track ") + track + String(" within folder ") + folder);
}

void spinWhells(int wheelOne, int wheelTwo, int wheelThree, int wheelFour) {} //TOADD: Implement this

//***************************OPTIONS & DIFFICULTY***************************
//Option Generator
/*Option format:
  [Index 0 <variant from 0 to 9, with 0 vs 9 being the biggest diff>
  1 <colour diff>
  2 <shape diff>
  3 <size diff>
  4 <pitch diff>
  5 <loudness diff>
  6 <timbre diff>
  7 <left vs right diff (0 is left, 9 is right)>
  8 <texture diff>
  9 <temp diff>]
*/

/*Difficulty format: [
  Index 0 <number from 0 to 2, with 0 being the easiest and 2 the hardest>
  1 <colour diff>
  2 <shape diff>
  3 <size diff>
  4 <pitch diff>
  5 <loudness diff>
  6 <timbre diff>
  7 <left vs right diff (0 is left, 9 is right)>
  8 <texture diff>
  9 <temp diff>]
*/

void OptionsGenerator(String difficulty, String resultOptions[3]) { //Places options in resultOptions, a 3 element array with first dimension being the options, and second dimension being the details defining each option  
  //* is the pointer to where the data is actually located
  //TOADD: >3 options
  //TOADD: difficulty >0
  if (difficulty.length() != 10) { //difficulty String must be 10 char
    errorMessage(String(String("OptionsGenerator input not 10 char: ") + difficulty).c_str());
  }

  //String resultOptions[3];
  int differentOptionIndex = random(0, 3); //Random int from 0 to 2 (function returns 0<=x<3)
  debugMessage(String(String("different option (0-2): ") + differentOptionIndex).c_str());

  //deep copy the difficulty into all 3 options
  for (int i = 0; i <= 2; i++) {
    resultOptions[i] = String(difficulty);
  }
  
  //modify the first character of each sub-array to fit that of the option format (only difference between option and difficulty format is the first character: difficulty vs variant)
  for (int i = 0; i <= 2; i++) {
    if (i == differentOptionIndex) {
      resultOptions[differentOptionIndex].setCharAt(0, '9'); //Change the different option's variant
    }
    else { //The "normal" options
      resultOptions[i].setCharAt(0, '0'); //TOADD: For now will always be 0, in future should depend on difficulty
    }
    debugMessage(String(String("Option ") + i + String("=") + resultOptions[i]).c_str()); //Cannot "+"" char[] and numbers
  }
  
  //return resultOptions;
}

//***************************GAME***************************
/*class gameOption {
  private:
  public:
    bool isInitialised = false;
    int colourVariant = -1;
    int shapeVariant = -1;
    int sizeVariant = -1;
    int pitchVariant = -1;
    int loudnessVariant = -1;
    int timbreVariant = -1;
    int leftRightVariant = -1; //0 is left, 9 is right
    int textureVariant = -1;
    int tempvariant = -1;
      

    gameOption(String setupString) {
      //if (setupString.length() != 
      //setupStringCharArray[10] = {NULL};
      //setupString.toCharArray();
      int colourVariant = -1;
      int shapeVariant = -1;
      int sizeVariant = -1;
      int pitchVariant = -1;
      int loudnessVariant = -1;
      int timbreVariant = -1;
      int leftRightVariant = -1; //0 is left, 9 is right
      int textureVariant = -1;
      int tempvariant = -1;
      
    }

    gameOption(char *setupString) {

    }



};*/

class gameRound { //Stores one round (ie one choice)
  private:
  public:
    bool isInitialised = false;
    String difficulty = String();
    String options[3] = {String(), String(), String()}; //3 element array of the options in the relevant format
    int optionChosen = NULL;

    gameRound(String passedInDifficulty) {
      difficulty = String(passedInDifficulty); //Deep copy because OptionsGenerator (next line) will modify the String object to "return" the output
      OptionsGenerator(difficulty, options);
      isInitialised = true;

      /*debugMessage(options[0].c_str());
      debugMessage(options[1].c_str());
      debugMessage(options[2].c_str());*/
    }
    gameRound() {}
};

class gameOverall { //Stores multiple game rounds
  //TOADD: More than 4 game rouns
  private:
  public:
    String arrayOfDifficulties[4];
    gameRound arrayOfRounds[4];

    gameOverall(String initialDifficulty) {
      for (int i = 0; i<4; i++) {
        arrayOfDifficulties[i] = String(initialDifficulty); //Go back and check if rly need this deep copy
        debugMessage(String("Round" + String(i) + "difficulty: " + arrayOfDifficulties[i]));
        arrayOfRounds[i] = gameRound();
      }
    }

    //TOADD: Keeping score
};

//***************************WiFi & MQTT***************************

void setupWIFI() {
  WiFi.setHostname(hostname);
  Serial.print("Hostname: ");
  Serial.println(hostname);

  Serial.print("Connecting to ");
  Serial.print(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void setupMQTT() { //Also subscribes to the appropriate MQTT topics for fleet or lift adapters
  //Config MQTT
  MQTTclient.setServer(mqttServerIP, 1883); //mqtt_server IP address
  MQTTclient.setCallback(receiveMQTTCallback);

  //Connect to MQTT server
  Serial.print("Trying to connect to MQTT server");
  while (!MQTTclient.connected()) {
    Serial.print(".");
    MQTTclient.connect(clientID);
    delay(500);
  }
  Serial.println();
  Serial.print("Connected to MQTT Server on ");
  Serial.println(mqttServerIP);
  
  //Subscriptions
  MQTTclient.subscribe(settings_topic);
  MQTTclient.subscribe(espCommands_topic);
}

void receiveMQTTCallback(char* topic, byte* message, unsigned int length) { //Do the messages receive end in null?
  Serial.println("****received MQTT transmission****");
  //Copy in the MQTT message
  char messageTemp[256] = {NULL};
  for (int i = 0; i < length; i++) {
    messageTemp[i] = (char)message[i];
  }
  String messageString = String(messageTemp);
  //Print to serial
  Serial.print("Topic: ");
  Serial.println(topic);
  Serial.print("Message: ");
  Serial.println(messageTemp);

  //Handle them
  if(String(topic) == espCommands_topic) {
    if(messageString.substring(0, 3) == "LED") { //The starting index is inclusive (the corresponding character is included in the substring), but the optional ending index is exclusive (the corresponding character is not included in the substring)
      if(messageString.substring(4, 5) == "1") {
        ledStrip(1);
      }
      else if(messageString.substring(4, 5) == "0") {
        ledStrip(0);
      }
    }
  }

  if(String(topic) == espCommands_topic) {
    if(messageString.substring(0, 3) == "MTR") { //The starting index is inclusive (the corresponding character is included in the substring), but the optional ending index is exclusive (the corresponding character is not included in the substring)
      if(messageString.substring(4, 5) == "1") {
        vibrationMotor(1);
      }
      else if(messageString.substring(4, 5) == "0") {
        vibrationMotor(0);
      }
    }
  }
}

void MQTTpublishWithSerial(const char* topic, const char* payload) {
  Serial.println("----publishing MQTT transmission----");
  Serial.print("topic: ");
  Serial.println(topic);
  Serial.print("payload: ");
  Serial.println(payload);
  MQTTclient.publish(topic, payload);
}

void MQTTloop(void* pvParameters) { //Core for 2nd core (core 1) that constantly checks for received MQTT posts
  while(true) {
    if (!MQTTclient.connected()) {
      setupMQTT();
    }

    //Run the loop to ensure messages are received
    MQTTclient.loop();
    delay(100); //Delay is needed else this task interferes with IR transmitting
  }
}

//***************************interrupts --> callbacks for buttons***************************
//UNRELIABLE. Possibly because the buttons not debounced or callbacks change the values faster than the loop() function can
//0 : MQTT_CONNECTED - the client is connected
//MQTTclient.state()

void switchCallbacks(int switchNum, bool endDigitalRead) { //called when digital input goes from high to low or vice versa, switchNum is 1 indexed
  if (endDigitalRead) { //if current state high, means pin went from low to high, means went from depressed to released
    buttonFlags[switchNum-1] = -1; //-1 indicates that it got "unpressed"
  }
  else { //went from high to low means from released to depressed
    buttonFlags[switchNum-1] = 1;
  }
}

void ARDUINO_ISR_ATTR SW1_callback() { //Called when the button goes from pressed to unpressed or vice versa
  switchCallbacks(1, digitalRead(SW1_PIN));

}

void ARDUINO_ISR_ATTR SW2_callback() { //Called when the button goes from pressed to unpressed or vice versa
  switchCallbacks(2, digitalRead(SW2_PIN));
}

void ARDUINO_ISR_ATTR SW3_callback() { //Called when the button goes from pressed to unpressed or vice versa
  switchCallbacks(3, digitalRead(SW3_PIN));
}

void ARDUINO_ISR_ATTR SW4_callback() { //Called when the button goes from pressed to unpressed or vice versa
  switchCallbacks(4, digitalRead(SW4_PIN));
}

//***************************SETUP & LOOP***************************
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
  pinMode(SW2_PIN, INPUT_PULLUP);
  pinMode(SW3_PIN, INPUT_PULLUP);
  pinMode(SW4_PIN, INPUT_PULLUP);

  //***************************INITIALISATION***************************
  audioModule.setVolume(30);
  Serial.begin(115200);

  setupWIFI();
  setupMQTT();

  //***************************ATTACH INTERRUPTS***************************
  attachInterrupt(SW1_PIN, SW1_callback, CHANGE);
  attachInterrupt(SW2_PIN, SW2_callback, CHANGE);
  attachInterrupt(SW3_PIN, SW3_callback, CHANGE);
  attachInterrupt(SW4_PIN, SW4_callback, CHANGE);
  debugMessage("Interrupts attached");

  //OptionsGenerator(String("0110111110"));
  /*gameRound firstGame = gameRound(String("0110111110"));
  debugMessage(firstGame.options[0].c_str());
  debugMessage(firstGame.options[1].c_str());
  debugMessage(firstGame.options[2].c_str());
  if (firstGame.optionChosen == NULL) {
    debugMessage("No option chosen yet");
  }*/

  gameOverall game = gameOverall("0110111110");
  xTaskCreate(MQTTloop, "MQTTlooping", 10000, NULL, 4, &MQTTloopTask);
}

void loop() {
  delay(1000);
  for (int i = 0; i<4; i++) {
    if(buttonFlags[i] != 0) { //there is a button pressed or released
      if(buttonFlags[i] == 1) {
        debugMessage(String("Switch ") + int(i+1) + String(" pressed"));
        buttonFlags[i] = 0;
      }
      
      else { //button pressed
        debugMessage(String("Switch ") + int(i+1) + String(" unpressed"));
        buttonFlags[i] = 0;
      }
    }
  }
  // put your main code here, to run repeatedly:
  /*if (digitalRead(SW1_PIN) == 1) {
    audioModule.playTrackInFolder(1, 1);
    digitalWrite(LED_PIN, 1);String("0110111110")
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
  }*/
}

