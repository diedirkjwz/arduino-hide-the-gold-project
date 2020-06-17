//----------Libraries----------
//This are all the libraries used for the project

//NFC
#include <SPI.h>
#include <MFRC522.h>
#include "Player.h"
class Player;

//Encoder
#include "EncoderStepCounter.h"

//Button
#include <Bounce2.h>

//LED
#include <Adafruit_NeoPixel.h>

//GPS
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

//OLED
#include <U8g2lib.h>
#include <Arduino.h>
//#include <avr/wdt.h>

//----------Pins & Variables----------
//This are the configurable pins on the arduino

//NFC
#define RST_PIN         9
#define SS_PIN          10

//Buzzer
#define buzzer_PIN      A1

//Encoder
#define ENCODER_PIN1 8  //encoder data pins pin 8 and a2
#define ENCODER_PIN2 16

//LED
#define PIN 2     //data pin
#define LED_COUNT  5    //amount of leds

//Button
#define BUTTON 5
byte button_pins[] = {5};
#define NUMBUTTONS sizeof(button_pins)

//GPS
float distancebetweenpoints = 0;
float totaldistance = 0;
float startlat = 0;
float startlng = 0;
//float endlat = 0;
//float endlng = 0;


//OLED
char buf[10]; // text buffer; to be able to use draw2x2String to show the value
char char1 = '-'; // first character of command to receive via chat

//int menuhelper = 0;
int playercount = 4;
int fieldsize = 3;    //1=small 2=medium 3=large
int setupstep = 1;

String braceletColour = "";

//Variables used for the whole game
int thievesAmount = 3; //Change this line when interface is done
int thieves = 0; //How many thieves there are right now
int copsAmount = 1;
int cops = 0; //How many cops there are right now
int NUM_PLAYERS = copsAmount + thievesAmount;
int targetDistance = 1000; //Determined in the setup
int totalDistance = 0;
int fieldsize_small = 500;
int fieldsize_medium = 1000;
int fieldsize_large = 2500;

String UIDRED = "E1 CE 59 01";
String UIDBLUE = "F6 43 62 32";
String UIDGREEN = "9999";

Player * players[7]; //An array is created for all the thieves

//----------Create instances----------
//Here instances of classes are created that are defined in the libraries

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 (NFC class) instance.
EncoderStepCounter encoder(ENCODER_PIN1, ENCODER_PIN2);  // Create instance for one full step encoder
Bounce * buttons = new Bounce[NUMBUTTONS]; //Create Bounce button instance
Adafruit_NeoPixel strip = Adafruit_NeoPixel(5, PIN, NEO_GRB + NEO_KHZ800); //Create LED instance
TinyGPSPlus gps; //GPS instance
U8X8_SSD1306_128X64_NONAME_HW_I2C display(U8X8_PIN_NONE); //OLED instance
SoftwareSerial ss(4, 3);
//----------Main code----------

void setup() {
  Serial.begin(9600); //Start connection with serial monitor
  ss.begin(9600);
  Serial.println("Connected");

  //Setup functions of each component
  NFC_setup ();
  ENCODER_setup();
  Piezo_startup();
  BUTTON_setup();
  LED_setup();

  Game_Setup_Main();

  DISPLAY_setup();
  GAME_setup();
  GPS_setup ();
  //Settup function for the game itself
}

void loop() {
  Game_CheckButton();
  Game_KeepDistance ();
}


//----------Functions----------

//______________Game_Setup______________
//Here are the functions that are called in the setup phase of the game, when the user has to change settings

void Game_Setup_Main () {                  //This function is a setup function for the whole game
  for (int i = 0; i < thievesAmount; i++) {
    players[i] = NULL;
  }
}

void Game_Setup_team_divider() {        //This function divides the teams according to player count

  if (playercount == 2) {
    thievesAmount = 1; //Change this line when interface is done
    copsAmount = 1;
  }
  else if (playercount == 3) {
    thievesAmount = 2; //Change this line when interface is done
    copsAmount = 1;
  }
  else if (playercount == 4) {
    thievesAmount = 3; //Change this line when interface is done
    copsAmount = 1;
  }
  else if (playercount == 5) {
    thievesAmount = 3; //Change this line when interface is done
    copsAmount = 2;
  }
  else if (playercount == 6) {
    thievesAmount = 4; //Change this line when interface is done
    copsAmount = 2;
  }
  else if (playercount == 7) {
    thievesAmount = 4; //Change this line when interface is done
    copsAmount = 3;
  }
  else if (playercount == 8) {
    thievesAmount = 5; //Change this line when interface is done
    copsAmount = 3;
  }
  else if (playercount == 9) {
    thievesAmount = 6; //Change this line when interface is done
    copsAmount = 3;
  }
  else if (playercount == 10) {
    thievesAmount = 6; //Change this line when interface is done
    copsAmount = 4;
  }
  else if (playercount == 11) {
    thievesAmount = 7; //Change this line when interface is done
    copsAmount = 4;
  }
  else if (playercount == 12) {
    thievesAmount = 7; //Change this line when interface is done
    copsAmount = 5;
  }

}


int Game_Setup_NewPlayer_Random () {    //This function adds a new player if the teams are made at random
  if (thieves == thievesAmount && cops == copsAmount) {
    //teams are full!
    return 0;
  }
  else if (thieves == thievesAmount && cops != copsAmount) {
    //put player in cops
    //Displays on screen that you are a cop
    cops++;
    return -1;
  }
  else if (thieves != thievesAmount && cops == copsAmount) {
    //put player in thieves
    //call function of interface that lets you select the color and scan UID. Then call addPlayer(UID, true, Color);
    thieves++;
    return 1;
  }
  else {
    int t = random(0, 2);
    if (t == 0) {
      //put player in thieves
      //same as above
      thieves++;
      return 1;
    }
    else {
      //put player in cops
      //Same as above
      cops++;
      return -1;
    }
  }
}

void Game_Setup_NewPlayer_notRandom (bool thief) {    //This function adds a new player if the players decide themselves what the teams are
  if (thief == false) {
    if (cops == copsAmount) {
      //cops are full
    }
    //put player in cops
    //same as above
    cops++;
  }
  else if (thief == true) {
    if (thieves == thievesAmount) {
      //thieves are full
    }
    //put player in thieves
    //same as above
    thieves++;
  }
}

void addPlayer(String u, bool t, String c) {      //This function adds a new player to the array list and takes the UID, color and thieve/cops status as input
  bool added = false;
  for (int i = 0; i < thievesAmount; i++) { //For all places in the array (number of players)
    if (players[i] == NULL) {             //If this place is free (no thief has been added yet)
      players[i] = new Player(u, t, c);   //Add a thief to this place
      added = true;
    }
  }
  if (!added) {
    //list if full
  }
}

/*
  void GAME_reset () {  //This function resets the game
  if (menuhelper == -1) {
    void(* resetFunc) (void) = 0; //declare reset function @ address 0
    resetFunc();  //call reset
  }
  }
**/

void GAME_setup() {           //This is the main setup function initiates the game
  int menuhelper = 0;
  while (setupstep < 8) {
    if (setupstep == 1) { //begin game?
      display.clear();
      display.draw2x2String(4, 0, "PLAY");
      display.drawString(1, 3, "Hide the gold?");
      display.drawString(5, 7, "Yes  No");
      display.drawString(4, 7, "-");
      while (digitalRead(BUTTON) == HIGH) {
        if (ENCODER_change(2) == -1) {  //go to no
          display.drawString(4, 7, " ");
          display.drawString(9, 7, "-");
          menuhelper = -1;
          //Serial.println("goodbye");
        }
        else if (ENCODER_change(2) == 1) {  //go to yes
          display.drawString(4, 7, "-");
          display.drawString(9, 7, " ");
          menuhelper = 0;
          //Serial.println("hello");
        }
      }
      if (menuhelper == -1) {
        setupstep = 1;
      }
      else if (menuhelper == 0)
      {
        setupstep = 2;
      }
    }

    else if (setupstep == 2) { //amount of players
      display.clear();
      menuhelper = 6;
      display.drawString(3, 0, "number of");
      display.drawString(4, 2, "players:");
      display.drawString(1, 7, "back");
      display.setCursor(8, 5);
      display.print(menuhelper);
      delay (100);
      while (digitalRead(BUTTON) == HIGH) {
        if (ENCODER_change(2) == 1) {  //go to no
          menuhelper = menuhelper - 1;
          display.setCursor(8, 5);
          display.print("  ");
          display.drawString(0, 7, " ");
          display.setCursor(8, 5);
          display.print(menuhelper);
          playercount = menuhelper;
          if (menuhelper == 1 ) {  //select back button
            display.drawString(0, 7, "-");
            display.setCursor(8, 5);
            display.print("  ");
          }
          else if (menuhelper < 1 ) {
            menuhelper = 1;
            display.setCursor(8, 5);
            display.print("  ");
            display.setCursor(8, 5);
            display.print(menuhelper);
          }

        }
        else if (ENCODER_change(2) == -1) {  //go to yes
          menuhelper = menuhelper + 1;
          display.setCursor(8, 5);
          display.print("  ");
          display.drawString(0, 7, " ");
          display.setCursor(8, 5);
          display.print(menuhelper);
          playercount = menuhelper;
          if (menuhelper == 12 ) {  //select back button
            display.drawString(0, 7, "-");
            display.setCursor(8, 5);
            display.print("  ");
          }
          else if (menuhelper > 12 ) {
            menuhelper = 12;
            display.setCursor(8, 5);
            display.print("  ");
            display.setCursor(8, 5);
            display.print(menuhelper);
          }
        }

      }
      if (menuhelper == 12 || menuhelper == 1) {
        setupstep = 1;
      }
      else {
        setupstep = 3;
      }
      Game_Setup_team_divider();

    }

    else if (setupstep == 3) { //field size
      display.clear();
      menuhelper = 2;
      display.drawString(3, 0, "field size:");
      display.drawString(6, 2, "small");  //size of fields can be adjusted
      display.drawString(6, 4, "medium");
      display.drawString(5, 4, "-");
      display.drawString(6, 6, "large");
      display.drawString(1, 7, "back");
      delay (100);
      while (digitalRead(BUTTON) == HIGH) {

        if (ENCODER_change(2) == 1) {  //go to no
          display.drawString(5, 2, " ");
          display.drawString(5, 4, " ");
          display.drawString(5, 6, " ");
          display.drawString(0, 7, " ");
          menuhelper = menuhelper - 1;
          if (menuhelper == 1)
          {
            fieldsize = menuhelper;
            display.drawString(5, 2, "-");
            targetDistance = fieldsize_small;
          }
          else if (menuhelper == 2)
          {
            fieldsize = menuhelper;
            display.drawString(5, 4, "-");
            targetDistance = fieldsize_medium;
            //Serial.println(fieldsize);
            //Serial.println(menuhelper);
          }
          else if (menuhelper == 3)
          {
            fieldsize = menuhelper;
            display.drawString(5, 6, "-");
            targetDistance = fieldsize_large;
          }
          if (menuhelper == 0 ) {  //select back button
            display.drawString(0, 7, "-");
            display.setCursor(8, 5);
            display.print("  ");
          }
          else if (menuhelper < 0 ) {
            menuhelper = 0;
          }

        }
        else if (ENCODER_change(2) == -1) {  //go to yes
          display.drawString(5, 2, " ");
          display.drawString(5, 4, " ");
          display.drawString(5, 6, " ");
          display.drawString(0, 7, " ");
          menuhelper = menuhelper + 1;

          if (menuhelper == 1)
          {
            fieldsize = menuhelper;
            display.drawString(5, 2, "-");
          }
          else if (menuhelper == 2)
          {
            fieldsize = menuhelper;
            display.drawString(5, 4, "-");
          }
          else if (menuhelper == 3)
          {
            fieldsize = menuhelper;
            display.drawString(5, 6, "-");
          }

          if (menuhelper == 4 ) {  //select back button
            display.drawString(0, 7, "-");
            display.setCursor(8, 5);
            display.print("  ");
          }
          else if (menuhelper > 4 ) {
            menuhelper = 4;
          }

        }

        if (menuhelper == 0 || menuhelper == 4) {
          setupstep = 2;
        }
        else {
          setupstep = 4;
        }


      }


    }

    if (setupstep == 4) { //manual or random teams
      menuhelper = 1;
      display.clear();
      display.drawString(4, 0, "TIME TO");
      display.drawString(3, 4, "MAKE TEAMS!");
      delay (1000);
      display.clear();
      display.drawString(1, 0, "How do you want");
      display.drawString(1, 1, "to make teams?");
      display.drawString(1, 4, "Random   Manual");
      display.drawString(0, 4, "-");
      display.drawString(1, 7, "back");
      while (digitalRead(BUTTON) == HIGH) {
        if (ENCODER_change(2) == -1) {  //go to no
          menuhelper = menuhelper + 1;
          if (menuhelper == 1)
          {
            display.drawString(0, 4, "-");
            display.drawString(9, 4, " ");
            display.drawString(0, 7, " ");
          }
          else if (menuhelper == 2)
          {
            display.drawString(0, 4, " ");
            display.drawString(9, 4, "-");
            display.drawString(0, 7, " ");
          }
          else if (menuhelper == 3 || menuhelper == 0)
          {
            display.drawString(0, 4, " ");
            display.drawString(9, 4, " ");
            display.drawString(0, 7, "-");
          }
          else if (menuhelper < 0 ) {
            menuhelper = 0;
          }
          else if (menuhelper > 3 ) {
            menuhelper = 3;
          }
        }
        else if (ENCODER_change(2) == 1) {  //go to yes
          menuhelper = menuhelper - 1;
          if (menuhelper == 1)
          {
            display.drawString(0, 4, "-");
            display.drawString(9, 4, " ");
            display.drawString(0, 7, " ");
          }
          else if (menuhelper == 2)
          {
            display.drawString(0, 4, " ");
            display.drawString(9, 4, "-");
            display.drawString(0, 7, " ");
          }
          else if (menuhelper == 3 || menuhelper == 0)
          {
            display.drawString(0, 4, " ");
            display.drawString(9, 4, " ");
            display.drawString(0, 7, "-");
          }
          else if (menuhelper < 0 ) {
            menuhelper = 0;
          }
          else if (menuhelper > 3 ) {
            menuhelper = 3;
          }
        }
      }
      if (menuhelper == 0 || menuhelper == 3) {
        setupstep = 3;
      }
      else if (menuhelper == 1)
      {
        setupstep = 5;
      }
      else if (menuhelper == 2)
      {
        setupstep = 6;
      }
    }

    if (setupstep == 5) {                                              //random teams
      menuhelper = 1;
      display.clear();
      display.drawString(0, 0, "Press the Button");
      display.drawString(0, 2, "to receive your");
      display.drawString(0, 4, "random role!");
      delay(100);
      while (menuhelper != 0) {
        menuhelper = Game_Setup_NewPlayer_Random ();
        if (menuhelper == 1) {           //thieves
          display.clear();
          display.drawString(0, 0, "You are a thief");
          display.drawString(0, 2, "scan your");
          display.drawString(0, 4, "bracelet");
          delay (1000);
          Serial.println("check2");
          while (NFC_checkUID() == "nothing") {
            display.clear();
            Serial.println("check3");
            display.drawString(0, 0, "waiting for");
            display.drawString(0, 2, "bracelet");
          }
          if (NFC_checkUID() != "nothing") {
            if (NFC_checkUID() == UIDRED) {
              braceletColour = "red";
              //LED_setColor(red);
            }
            else if (NFC_checkUID() == UIDBLUE) {
              //braceletColour = "blue";
              //LED_setColor(blue);
            }
            else if (NFC_checkUID() == UIDGREEN) {
              //braceletColour = "green";
              //LED_setColor(green);
            }
            else {
              display.clear();
              display.drawString(0, 0, "Bracelet not ");
              display.drawString(0, 2, "recognized");
              delay(1000);
              return;
            }
            addPlayer(NFC_checkUID(), true, braceletColour);
          }
          while (digitalRead(BUTTON) == HIGH) {}
        }
        else if (menuhelper == -1) //cop
        {
          display.clear();
          display.drawString(0, 0, "You are a cop");
          while (digitalRead(BUTTON) == HIGH) {}
        }
        //LED_setColor(off);

      }
      delay(1000);
      setupstep = 7;
    }
    if (setupstep == 6) {                                              //manual teams
      menuhelper = thievesAmount;
      display.clear();
      display.drawString(0, 0, "please make the");
      display.drawString(0, 2, "teams, there are");
      display.drawString(0, 4, "  cops and");
      display.drawString(0, 6, "  thieves");
      display.setCursor(0, 4);
      display.print(copsAmount);
      display.setCursor(0, 6);
      display.print(thievesAmount);
      delay(5000);
      while (menuhelper > 0) {
        if (digitalRead(BUTTON) == LOW) {
          display.clear();
          display.drawString(0, 0, "Please scan");
          display.drawString(0, 2, "your bracelet");
          delay (1000);
          while (NFC_checkUID() == "nothing") {
            display.clear();
            display.drawString(0, 0, "waiting for");
            display.drawString(0, 2, "bracelet");
          }
          if (NFC_checkUID() != "nothing") {
            if (NFC_checkUID() == "1234") {
              braceletColour = "red";
              //LED_setColor(red);
              delay (10);
            }
            else if (NFC_checkUID() == "6789") {
              braceletColour = "blue";
              //LED_setColor(blue);
              delay (10);
            }
            else if (NFC_checkUID() == "4321") {
              braceletColour = "green";
              //LED_setColor(green);
              delay (10);
            }
            else {
              display.clear();
              display.drawString(0, 0, "Bracelet not ");
              display.drawString(0, 2, "recognized");
              delay(1000);
              return;
            }
            addPlayer(NFC_checkUID(), true, braceletColour);
            menuhelper = menuhelper - 1;
          }
          //LED_setColor(off);
        }
      }
    }
    if (setupstep == 7) {
      display.clear();
      display.drawString(0, 0, "The teams have");
      display.drawString(0, 2, "been formed!");
      display.drawString(0, 4, "Press the OK");
      display.drawString(0, 6, "button to start!");
      while (digitalRead(BUTTON) != LOW) {
        delay(1);
      }
      display.clear();
      setupstep = 8;
    }
  }
}



//______________Main_Game______________
//These functions are run multiple times during the game

void Game_CheckButton () {                //This functions checks if the button is pressed for 2 seconds
  if (digitalRead(BUTTON) == LOW) {
    delay(2000);
    if (digitalRead(BUTTON) == LOW) {
      display.clear();
      display.drawString(0, 4, "COPS WIN!");
      delay(10000);
      display.clear();
      //GAME_reset ();
    }
  }
}

void Game_KeepDistance () {             //This functions keeps the distance walked with the device and calls the Change_thief() and Thieves_win() function when needed

  GPS_adddistance(); //Let the GPS function refresh the total distance
  display.clear();
  display.setCursor(0, 4);
  int remainingDistance = targetDistance - totalDistance;
  Serial.print("remainingDistance:");
  Serial.println(remainingDistance);
  display.print(remainingDistance);
  //Show total distance on screen
  int changeDistance = targetDistance / thievesAmount; //Device the total distance to win by the amount of thieves (rounded of to integers)
  int x = 1;
  if (totalDistance > x * changeDistance && totalDistance < targetDistance) { //If one 'change distance' is walked
    Change_thief(); //The thieves have to change
    //Change thief screen
    x++;
  }
  if (totalDistance > targetDistance) {
    Thieves_win(); //If the total distance is walked, the thieves win
    //Win screen
    display.clear();
    display.drawString(0, 4, "THIEVES WIN!");
    delay(10000);
    display.clear();
    //GAME_reset ();
  }
}

void Change_thief () {                  //This function is called when the thief who carries the device has to hand it over to a new thief
  boolean chosen = false;
  while (chosen == false) {
    int Number = random(0, thievesAmount + 1); //Choose a random thief

    if (players[Number] != NULL && players[Number]->getCarried() == false) {  //Only thieves who have not carried the device may carry it. If this thief has not carried it:
      chosen = true;  //A thief has been chosen
      while (NFC_checkUID() != players[Number]->getUID()) {  //While the NFC scanner does not scan the UID of the selected thief:
        LED_setColor(players[Number]->getColor()); //The device flickers in the color of the thief and produces a biep
        delay(200);
        LED_setColor("off");
        Piezo_play (800, 200);
      }
      LED_setColor(players[Number]->getColor()); //When the scanner does scan the UID, a tone will be played and the device will remain in the color of the thief. Then the game goes on
      Piezo_play (800, 200);
      Piezo_play (400, 200);
      players[Number]->setCarried(true);
    }
  }
}

void Thieves_win () { //This function is called when the thieves win (if they have traveled a certain distance)
  //some nice flashes et
}

//______________Button______________
//These functions are to check if the button is pressed

void BUTTON_setup () //Setup function or the button
{
  for (int i = 0; i < NUMBUTTONS; i++) {
    buttons[i].attach( button_pins[i], INPUT_PULLUP);
    buttons[i].interval(20);
  }
  pinMode(BUTTON, INPUT_PULLUP); //set the button to output
  Serial.println("Button Connected");
}

void BUTTON_Check () //check the state of the button
{
  if (digitalRead(BUTTON) == HIGH) {
    return;
  }
}

//______________OLED______________

void DISPLAY_setup() {
  display.begin();
  display.setFlipMode(1);
  display.setPowerSave(0);
  display.setFont(u8x8_font_pxplusibmcgathin_f);
}

//______________NFC______________
//These functions check if the NFC receives signal, and will return the identification code (UID) of cards

void NFC_setup () //Setup function of the NFC scanner
{
  Serial.println("NFC Connected");
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
}

String NFC_checkUID() {   //Function that checks wheather a NFC card is present, and returns the UID
  if (mfrc522.PICC_IsNewCardPresent()) { //If a new car is present
    if (mfrc522.PICC_ReadCardSerial()) { //And this card can be read
      Serial.println("Card!");           //Let the serial monitor know a card has been found

      // Show UID on serial monitor
      String uid = "";
      for (byte i = 0; i < mfrc522.uid.size; i++) {
        uid.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
        uid.concat(String(mfrc522.uid.uidByte[i], HEX));
      }
      uid.toUpperCase();
      uid = uid.substring(1);
      Serial.print("UID tag: "); Serial.println(uid);

      return uid;                       //Return the uid of the card
    }
    else {
      return "nothing";                 //If no card is present, the string "Nothing" will be send back
    }
  }
  else {
    return "nothing";                   //If the card can't be read, the string "Nothing" will be send back
  }
}


//______________Piezo______________
//These functions are for the different sounds that the device has to produce

void Piezo_play (int freq, int time) {  //Function that takes the frequentie that the buzzer produces and the time it is played
  tone (buzzer_PIN, freq);
  delay(time);
  noTone(buzzer_PIN);
}

void Piezo_startup() {                  //Startup sound consisting of three tunes
  Piezo_play (200, 200);
  Piezo_play (400, 200);
  Piezo_play (600, 200);
}

//______________LED______________
//These functions are made to controll the RGB LEDs and give them different colors
void LED_setup() {                      //Setup function for the LED
  strip.begin();
  strip.show();
  Serial.println("LED Connected");
}


void LED_setColor(String color)  {      //Function that takes a color as a string var, and changes the RGB LEDs to this color
  int r = 0;  // color code int
  int g = 0;  // color code int
  int b = 0;  // color code int
  if (color == "green") { //looks and adjust for color
    r = 0;
    g = 255;
    b = 0;
  }
  else if (color == "red") {  //looks and adjust for color
    r = 255;
    g = 0;
    b = 0;
  }
  else if (color == "blue") { //looks and adjust for color
    r = 0;
    g = 0;
    b = 255;
  }
  else if (color == "off") {  //looks and adjust for color
    r = 0;
    g = 0;
    b = 0;
  }

  for (uint16_t i = 0; i < strip.numPixels(); i++) { //use color on LEDS
    strip.setPixelColor(i, strip.Color(r, g, b));
    strip.show();
  }
}
//______________Encoder______________
//These functions are for reading the encoder



void ENCODER_setup () { //Setup function fo rthe encoder
  encoder.begin();
  Serial.println("ENCODER Connected");
}

int lastpos = 0;  //start lastpos variable

int ENCODER_givevalue() {
  //Call tick on every loop
  encoder.tick();
  // Print out current encoder value
  // Have a look at the InterruptExample on how
  // to have a bigger variable for the counter
  int pos = encoder.getPosition();

  return pos; //return the current position
}

int ENCODER_change (int changeValue) {
  int pos = ENCODER_givevalue();    //update the last position
  if ((-changeValue < pos) && (changeValue > pos)) {
    return 0;
  }
  else if (pos > changeValue) {
    encoder.reset();
    return 1;
  }
  else if (pos < -changeValue) {
    encoder.reset();
    return -1;
  }
}

void interrupt() {
  encoder.tick();
}


//______________GPS______________
//These functions are for receiving GPS signal and calculating distances


void GPS_setup() {
  //Everything to start the gps
  //ss.begin(9600);
  GPS_getcurrentlocation();
}

void GPS_getcurrentlocation() {
  //retrieves current location
  while (startlat == 0 || startlng == 0) {

    startlat = gps.location.lat();
    startlng = gps.location.lng();
    smartDelay(100);    //small delay for serial connection
  }
  startlat = gps.location.lat();
  Serial.println(startlat, 20);
  startlng = gps.location.lng();
  Serial.println(startlng, 20);
}
/*
  void GPS_calcdistance() {
  //calculation of distance travelled with GPS_getstartlocation and GPS_getcurrentlocation results in distancetravelled
  endlat = gps.location.lat();
  Serial.println(endlat, 20);
  endlng = gps.location.lng();
  Serial.println(endlng, 20);


  distancebetweenpoints = //distance in m
    (unsigned long)TinyGPSPlus::distanceBetween(
      endlat,
      endlng,
      startlat,
      startlng) / 1;
  Serial.println(distancebetweenpoints, 9);

  }
**/
void GPS_adddistance() {
  float endlat = 0;
  float endlng = 0;
  //calculation of distance travelled with GPS_getstartlocation and GPS_getcurrentlocation results in distancetravelled
  endlat = gps.location.lat();
  Serial.println(endlat, 20);
  endlng = gps.location.lng();
  Serial.println(endlng, 20);


  distancebetweenpoints = //distance in m
    (unsigned long)TinyGPSPlus::distanceBetween(
      endlat,
      endlng,
      startlat,
      startlng) / 1;
  Serial.println(distancebetweenpoints, 9);
  //GPS_calcdistance(); //Let the addistance function call the calcdistance and the getcurrentlocation function itself.
  //adds distancetravelled to totaldistance to create new totaldistance
  if (distancebetweenpoints > 5) {
    startlat = endlat;
    startlng = endlng;
    totaldistance = distancebetweenpoints + totaldistance;
    Serial.print("disttotal:");
    Serial.println(totaldistance);
  }
}

// This custom version of delay() ensures that the gps object
// is being "fed".
static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do
  {
    while (ss.available())
      gps.encode(ss.read());
  } while (millis() - start < ms);
}
