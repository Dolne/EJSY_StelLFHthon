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

//Option & difficulty variant indexes (dimensions)
const int INIT_INDEX = 0; //Initialised to 0 if n.a. or not initialised
const int COLOUR_INDEX = 1; //1: Blue, 9: Orange
const int SHAPE_INDEX = 2; //1: Triangle, 9: Square
const int SIZE_INDEX = 3; //1: Small, 9: Big
const int PITCH_INDEX = 4; //1: Low, 9: High
const int LOUDNESS_INDEX = 5; //1: Low, 9: High
const int TIMBRE_INDEX = 6; //1: Piano, 9: Flute
const int LR_INDEX = 7; //1: Left only, 9: Right only
const int TEXTURE_INDEX = 8; //1: Rough, 9: Smooth
const int TEMPERATURE_INDEX = 9; //1: Low, 9: High
const int TOTAL_INDEXES = 10;

const int MAX_OPTIONS = 4;

//Audio files
const int AUDIO_SCAN_OFFSET = 0; //TOADD AUDIO FILES
const int AUDIO_OPTIONS_OFFSET = 10; //TOADD AUDIO FILES
//Audio file index = 1+(Pitch==9)+2*(loudness==9)+4*(timbre)

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
const char* visualScan_topic = "display/selected";
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

int wasPressed[4] = {0, 0, 0, 0}; //is reset to 0 before i start waiting for a button press, respective index is changed to 1 when button is pressed, isnt reset to 0 till next time waiting for buttonpress

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

void errorMessage(String message) { //Send a debugging message out via MQTT & USB Serial
  Serial.println(message.c_str());
  MQTTpublishWithSerial(error_topic, message.c_str());
}

//***************************GAME***************************
//Game option = a String object
//Index 0: If option is initialised (0 or 1)
/*each remaining index's value:
  - 0=not showing a variant (e.g. no sound, no visual)
  - 1-9 corresponds to a variant
    - 1 and 9 being the most different
    - Check the const ints at the top for details
    - For prototype: Only 1 & 9.*/

//Difficulty = a String object
//Index 0: Always 0, not relevant //TOADD: CAN REVISIT
/*each remaining index's value:
  - 0 = not showing a variant (e.g. no sound, no visual) OR NO DIFFERENCE
  - 1-8 = degree of variance
    - 8 being greatest difference
    - Check the const ints at the top for details
    - For prototype: Only 0 or 8.
    */


/*Places options in resultOptions, a String array of length MAX_OPTIONS
Returns the index of the odd option*/
int OptionsGenerator(String difficulty, int numOptions, String resultOptions[MAX_OPTIONS]) { //TOADD: Missing handler for e.g. shape diff but colour not diff
  //* is the pointer to where the data is actually located

  //Create blank template for options
  char blankOption[TOTAL_INDEXES] = {NULL};
  for (int i = 0; i < TOTAL_INDEXES; i++) {
    blankOption[i] = '0';
  }

  //Validate inputs
  if (difficulty.length() != TOTAL_INDEXES) { //difficulty String must be 10 char
    errorMessage(String("Length of OptionsGenerator input isn't TOTAL_INDEXES, it's ") + difficulty);
  }

  if (!((numOptions == 3) || (numOptions == 4))) { //Right now only set up for 3 or 4 options
    errorMessage(String("Number of options requested isn't 3 or 4, it's ") + numOptions);
  }

  //Choose which option is different
  int differentOptionIndex = random(0, numOptions); //Random int from 0 to numOptions-1 (function returns 0<=x<numOptions-1)
  debugMessage(String("Different option index within resultOptions is ") + differentOptionIndex);

  //Initialise options
  for (int currentOption = 0; currentOption < MAX_OPTIONS; currentOption++) {
    //Set to "000..."
    resultOptions[currentOption] = String(blankOption);

    //If actual option
    if (currentOption < numOptions) {
      //change 0th index to 1 to initialise
      resultOptions[currentOption].setCharAt(INIT_INDEX, '1');
      //Other indexes (1-9)
      for (int indexWithinOption = 1; indexWithinOption < TOTAL_INDEXES, indexWithinOption++) { //Start from char @ index 1 cuz index 0 alrdy set to 1 to init
        if (difficulty.charAt(indexWithinOption) != 0) {//means this difference is activated
          if (currentOption == differentOptionIndex) { //If odd one out, set 9
            resultOptions[currentOption].setCharAt(indexWithinOption, '9');
          }
          else { //If others (not the odd 1 out), set 1
            resultOptions[currentOption].setCharAt(indexWithinOption, '1');
          }
        }
      }
    }
  }

  //Print out all options
  for (int i = 0; i < numOptions; i++) {
      debugMessage("Option " + String(i) + ": " + resultOptions[i])
  }

  return differentOptionIndex;
}

class gameRound { //Stores one round
  private:
  public:
    bool isInitialised = false;
    int numberOptions = NULL;
    int optionChosen = NULL;
    int correctOption = NULL;
    String difficulty = String();
    String options[4] = {String(), String(), String(), String()}; //should be string array of length MAX_OPTIONS to pass into OptionsGenerator
    

    gameRound(String passedInDifficulty, int numOptionsInput) {
      numberOptions = numOptionsInput;
      difficulty = String(passedInDifficulty); //Deep copy because OptionsGenerator (next line) will modify the String object to "return" the output
      correctOption = OptionsGenerator(difficulty, numOptionsInput, options);
      isInitialised = true;

      /*debugMessage(options[0].c_str());
      debugMessage(options[1].c_str());
      debugMessage(options[2].c_str());*/
    }
};

class gameOverall { //Stores multiple game rounds
  //TOADD: More or less than 4 game rouns
  private:
  public:
    String arrayOfDifficulties[4]; //one per round
    gameRound arrayOfRounds[4]; //one per round

    gameOverall(String initialDifficulty) { //initialDifficulty will be used for all rounds
      //TOADD: Not 4 rounds
      for (int i = 0; i<4; i++) { //set up each round
        arrayOfDifficulties[i] = String(initialDifficulty); //Go back and check if rly need this deep copy
        debugMessage(String("Round" + String(i) + "difficulty: " + arrayOfDifficulties[i]));
        arrayOfRounds[i] = gameRound(initialDifficulty, 4);
      }
    }


    //TOADD: Keeping score
};

int scanOption(int optionIndex, bool visual, bool audio) { //To highlight / say the option we are currently scanning (ie u will select if u press the button), optionIndex is 0 indexed
  if (visual) {
    MQTTpublishWithSerial(visualScan_topic, String(optionIndex+1).c_str());
  }

  if (audio) {
    playAudio(AUDIO_SCAN_OFFSET+optionIndex, 1);
  }
}
  
int OptionSelector(gameRound round, int typeOfSelecting) { //For prototype: only scanning

  int optionSelected = -1; //option 1 is optionSelected = 0
  //Display visuals (if any)
  //Generate the shape IDs
  /*1. small blue triangle
    2. large blue triangle
    3. small orange triangle
    4. large orange triangle
    5. small blue square
    6. large blue square
    7. small orange square
    8. large orange square*/
  //Thus, 4*shape + 2*colour + size

  //Check if any visual at all by seeing if option 1 all visual indexes is '0'
  bool hasVisual = !((round.options[1].charAt(SIZE_INDEX)=='0') && (round.options[1].charAt(COLOUR_INDEX)=='0') && (round.options[1].charAt(SHAPE_INDEX)=='0'));
  int shapeIDs [4] = [1, 1, 1, 1]; //Hardcoded 4 wheels oops //TOADD
  if (hasVisual) {
    //Generate the shape IDs to send to raspi via MQTT
    //TOADD: Missing handler for e.g. shape diff but colour not diff
    for (int i = 0; i < round.numberOptions; i++) {
      if(round.options[i].charAt(SIZE_INDEX) == '9') {
        shapeIDs[i] += 1;
      }
      if(round.options[i].charAt(COLOUR_INDEX) == '9') {
        shapeIDs[i] += 2;
      }
      if(round.options[i].charAt(SHAPE_INDEX) == '9') {
        shapeIDs[i] += 4;
      }
    }
  }
  else {
    shapeIDs [4] = [9, 9, 9, 9]; //This will make each wheel display its slot number (e.g. "1")
    //CAN MAKE IT 0 IF WANT PARITY WTH AUDIO
  }
  
  //Display the visuals
  spinWheels(shapeIDs)

  //Check if any audio at all by seeing if option 1 all audio indexes is '0'
  bool hasAudio = !((round.options[1].charAt(PITCH_INDEX)=='0') && (round.options[1].charAt(LOUDNESS_INDEX)=='0') && (round.options[1].charAt(TIMBRE_INDEX)=='0'));
  //Generate the audio file IDs to send to MP3 module
  int audioIDs [4] = [1, 1, 1, 1]; //Hardcoded 4 wheels oops //TOADD
  //TOADD: Missing handler for e.g. tone diff but timbre not diff
  //TOADD: Left and right channel diff
  if (hasAudio) {
    for (int i = 0; i < round.numberOptions; i++) { //i is the current option
      if(round.options[i].charAt(PITCH_INDEX) == '9') {
        shapeIDs[i] += 1;
      }
      if(round.options[i].charAt(LOUDNESS_INDEX) == '9') {
        shapeIDs[i] += 2;
      }
      if(round.options[i].charAt(TIMBRE_INDEX) == '9') {
        shapeIDs[i] += 4;
      }
    }
  }
  else {
    audioIDs [4] = [0, 0, 0, 0];
  }


  //Actual scanning loop
  for (int currentOption = 0; currentOption < round.numberOptions; currentOption++) {
    //Do the scanning for this option
    scanOption(currentOption, 1, hasAudio); //TOADD: Scanning with only visual or audio
    //play option 1-4 audio
    if (hasAudio) {
      playAudio(AUDIO_OPTION_OFFSET+audioIDs[currentOption], 1);
    }

    //Reset waspressed
    wasPressed[4] = {0, 0, 0, 0};

    //Wait for someone to press
    long startTime = millis();
    long waitTime = 5000;

    while ((millis() - startTime) < waitTime) {
      delay(10); //TODO: See if need this
    }

    if(wasPressed[0] == 1) {
      optionSelected = currentOption;
    }
  }

  //modify round object with option selected
  round.optionChosen = optionSelected;
  //Return optionSelected
  return optionSelected
}

//***************************HARDWARE FUNCTIONS***************************
//These exist in place of directly calling the relevant functions to enable debugging & error messages
void ledStrip(bool onOff) { //Turn the LED Strip on or off
  digitalWrite(LED_PIN, onOff);
    if (onOff) {
      digitalWrite(LED_PIN, 1);
      debugMessage(String("LED Strip on"));
    }
    else {
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
  else {
    digitalWrite(VIBRATION_PIN, 0);
    debugMessage(String("Vibration motor off"));
  }
}

void playAudio(int track, int folder) { //Play audio from the Audio Module
  audioModule.playTrackInFolder(track, folder);
  debugMessage(String("MP3 Playing: Track ") + track + String(" within folder ") + folder);
}

void spinWheels(shapeIDs [4]) {

} //TOADD: Implement this

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

void setupMQTT() { //Also subscribes to the appropriate MQTT topics
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
  if(String(topic) == espCommands_topic) { //Handle commands to do hardware things

    //LED Strip
    if(messageString.substring(0, 3) == "LED") { //The starting index is inclusive (the corresponding character is included in the substring), but the optional ending index is exclusive (the corresponding character is not included in the substring)
      if(messageString.substring(4, 5) == "1") {
        ledStrip(1);
      }
      else if(messageString.substring(4, 5) == "0") {
        ledStrip(0);
      }
    }

    //Vibration Motor
    if(messageString.substring(0, 3) == "MTR") { //The starting index is inclusive (the corresponding character is included in the substring), but the optional ending index is exclusive (the corresponding character is not included in the substring)
      if(messageString.substring(4, 5) == "1") {
        vibrationMotor(1);
      }
      else if(messageString.substring(4, 5) == "0") {
        vibrationMotor(0);
      }
    }

    //MP3 Audio Player
    //FOR SIM: String messageString = String("MP3 1 2");
    if(messageString.substring(0, 3) == "MP3") { //The starting index is inclusive (the corresponding character is included in the substring), but the optional ending index is exclusive (the corresponding character is not included in the substring)
      int charDividing = messageString.indexOf(" ", 4); //The index of th character dividing the track number and folder number
      int trackNum = messageString.substring(4, charDividing).toInt();
      int folderNum = messageString.substring(charDividing).toInt();
      playAudio(trackNum, folderNum);
      /*FOR SIM: Serial.println(folderNum);
      Serial.println(trackNum);*/
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

    vTaskDelay(pdMS_TO_TICKS(10));
    //yield for 10 ms
    //delay(100);
  }
}

//***************************interrupts --> callbacks for buttons***************************
//UNRELIABLE. Possibly because the buttons not debounced or callbacks change the values faster than the loop() function can or the 100ms sleep in the mqtt loop was slowing it down
//Maybe have some thing that logs the history
//0 : MQTT_CONNECTED - the client is connected
//MQTTclient.state()

void switchCallbacks(int switchNum, bool endDigitalRead) { //called when digital input goes from high to low or vice versa, switchNum is 1 indexed
  if (endDigitalRead) { //if current state high, means pin went from low to high, means went from depressed to released
    buttonFlags[switchNum-1] = -1; //-1 indicates that it got "unpressed"
  }
  else { //went from high to low means from released to depressed
    buttonFlags[switchNum-1] = 1;
    wasPressed[switchNum-1] = 1;
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
  audioModule.setVolume(10);
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

  //Play music (for debug test)
  audioModule.playTrackInFolder(1, 1);
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

