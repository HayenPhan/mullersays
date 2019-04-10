// Code for Simon Says Arduino Wiring and Code Tutorial
// Video found at: https://www.youtube.com/watch?v=TJiz7PT21B4

// includes for http request to add score to the database
// Tim's Includes LCD
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
// Einde Includes Tim
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
ESP8266WiFiMulti WiFiMulti;
// end includes
LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); // LCD Def
// Define all the LED and Button pins
// {RED, GREEN, YELLOW, BLUE}
int buttons[] = {3, 5, 7, 9}; //The four button input pins
int leds[] = {2, 4, 6, 8};  // LED pins
//button pins Score invoeren Tim
const int button1 = 1;
const int button2 = 3;
//button variabelen
char letter1 = 65;
char letter2 = 65;
char letter3 = 65;
int letter = 1;



// set game variables
int sequence[100];
int largestIndex = 0;
int buttonPin = 2;
const int START = 0;
const int PLAY = 1;
const int GAMEOVER = 2;
int gameState;
int speed = 500;
boolean running = true;  // Flag to know is knightrider is running
long currentMillis = 0;
long previousMillis = 0;
// end game variables

// Score
int score = 0;

// the setup routine runs once when you press reset:
void setup() {

  Serial.begin(9600);
  
    // Tim's LCD Code
  //WE define our LCD 16 columns and 2 rows
  lcd.begin(16,2);
  lcd.backlight();//Power on the back light
  //lcd.backlight(); Power off the back light
    lcd.setCursor(0,0); //we start writing from the first row first column
  lcd.print( "test"); //16 characters poer line

  //making conection to wifi
  Serial.println();

  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }

  Serial.print("Try to start connection with stads-lab");
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP("Stads-Lab", "initialkey4iot");
  //end making conection to wifi

  for (int pin = 0; pin < 4; pin++)
  {
    pinMode(leds[pin], OUTPUT);
  }

  for (int pin = 0; pin < 4; pin++)
  {
    pinMode(buttons[pin], INPUT);
  }



  gameState = START; // Start the game with the start.
  randomSeed(analogRead(40)); // initializes the pseudo-random number generator,




}

// the loop routine runs over and over again forever:
void loop() {

  if (gameState == START)
  {
    waitToStart();
  }
  else if (gameState == PLAY)
  {

    showSequence();
    readSequence();
  }
  else if (gameState == GAMEOVER)
  {
    printenScore();
    blinkAll(5);
    naamInvoeren();
    gameState = START;
  }

}

void showSequence()
{

  //blinkRed(2);

  sequence[largestIndex] = random(0, 4);
  largestIndex++;

  for (int index = 0; index < largestIndex; index++)
  {
    delay(300);
    digitalWrite(leds[sequence[index]], HIGH);
    delay(700);
    digitalWrite(leds[sequence[index]], LOW);
  }
  //blinkGreen(2);

}

void readSequence()
{

  //blinkYellow(2);
  int positionPressed;
  boolean madeMistake = false;


  for (int index = 0; index < largestIndex & madeMistake == false; index++)
  {


    positionPressed = waitForButton(1000); // 0, 1, 2, or 3


    if (positionPressed == -1 | positionPressed != sequence[index])
    {
      madeMistake = true; // Exit the loop.
      gameState = GAMEOVER;
      Serial.println("Gameover!");

    } if (madeMistake == false) {
      Serial.println("Je hebt 1 punt gescoord");
      score++;

    }
  }

  Serial.println(score);

  //blinkBlue(2);
}



// Returns the position of Button pressed (0, 1, 2, or 3) or -1 if no button is pressed in the time period.
int waitForButton(int delay)
{
  int buttonPressed = -1;
  int input;
  boolean buttonBackUp = false;

  currentMillis = millis();     // The number of ms since the program started running
  previousMillis = currentMillis;   // Records the point when we start spinning the loop.

  // Keep spinning the loop until "delay" seconds have passed.
  while (currentMillis - previousMillis < delay & buttonBackUp == false)
  {
    // Read the button and record when it has been pushed down.
    for (int pin = 0; pin < 4 & buttonBackUp == false; pin++)
    {
      if (digitalRead(buttons[pin]) == HIGH)
      {
        buttonPressed = pin;

        // Show the LED pushed.
        digitalWrite(leds[pin], HIGH);

        // It is possible the button is still being pushed.
        // This loop spins until the button is let up.
        while (currentMillis - previousMillis < delay & buttonBackUp == false)
        {
          input = digitalRead(buttons[pin]);
          if (input == LOW)
          {
            buttonBackUp = true;
          }
          currentMillis = millis();
        }

        // Turn the LED pushed off.
        digitalWrite(leds[pin], LOW);

        // See if they took to long.
        if (currentMillis - previousMillis > delay)
        {
          buttonPressed = -1; // They took to long to let the button up so they lose.
        }
      }
    }

    currentMillis = millis();
  }

  return buttonPressed;
}

void waitToStart()
{
  int buttonPressed = -1;
  allOff();

  for (int pin = 0; pin < 4; pin++)
  {
    if (buttonPressed == -1)
    {
      digitalWrite(leds[pin], HIGH);
      buttonPressed = waitForButton(800);
      digitalWrite(leds[pin], LOW);
    }
  }

  if (buttonPressed != -1)
  {
    // A button was pushed so wait then start playing.
    delay(2000);
    largestIndex = 0; // Restart
    gameState = PLAY;
  }
}

// Turns all the LEDs off.
void allOff()
{
  for (int pin = 0; pin < 4; pin++)
  {
    digitalWrite(leds[pin], LOW);
  }
}

// Turns all the LEDs on.
void allOn()
{
  for (int pin = 0; pin < 4; pin++)
  {
    digitalWrite(leds[pin], HIGH);
  }
}

// Spins a loop until "delay" milliseconds passes.
// While the loop is spinning we keep looking at the value of the button to see if pushed.
boolean readAnyButton(int delay)
{
  boolean buttonDown = false;

  currentMillis = millis();     // The number of ms since the program started running
  previousMillis = currentMillis;   // Records the point when we start spinning the loop.

  // Keep spinning the loop until "delay" seconds have passed.
  while (currentMillis - previousMillis < delay & buttonDown == false)
  {
    // Read the button and record when it has been pushed down.
    for (int pin = 0; pin < 4; pin++)
    {
      if (digitalRead(buttons[pin]) == HIGH)
      {
        buttonDown = true;
      }
    }

    currentMillis = millis();
  }

  return buttonDown;
}

void blinkAll(int times)
{
  for (int count = 0; count < times; count++)
  {
    allOn();
    delay(300);
    allOff();
    delay(300);
  }
}

void blinkRed(int times)
{
  for (int count = 0; count < times; count++)
  {
    digitalWrite(leds[0], HIGH);
    delay(300);
    digitalWrite(leds[0], LOW);
    delay(300);
  }
}

void blinkGreen(int times)
{
  for (int count = 0; count < times; count++)
  {
    digitalWrite(leds[1], HIGH);
    delay(300);
    digitalWrite(leds[1], LOW);
    delay(300);
  }
}

void blinkYellow(int times)
{
  for (int count = 0; count < times; count++)
  {
    digitalWrite(leds[2], HIGH);
    delay(300);
    digitalWrite(leds[2], LOW);
    delay(300);
  }
}

void blinkBlue(int times)
{
  for (int count = 0; count < times; count++)
  {
    digitalWrite(leds[3], HIGH);
    delay(300);
    digitalWrite(leds[3], LOW);
    delay(300);
  }
}

//function for http request to add score to database.
int addScoreToDatabase(int scoretoadd, String name) {
  if ((WiFiMulti.run() == WL_CONNECTED)) {

    WiFiClient client;
    HTTPClient http;

    Serial.print("[HTTP] begin...\n");
    if (http.begin(client, "http://score.velfox.eu/score/score.php?score=" + String(scoretoadd) + "&password=oke&name=" + name)) {  // HTTP

      Serial.print("[HTTP] GET...\n");
      // start connection and send HTTP header
      int httpCode = http.GET();

      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTP] GET... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload = http.getString();
          Serial.println(payload);
        }
      } else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }

      http.end();
    } else {
      Serial.printf("[HTTP} Unable to connect\n");
    }
  }
}

int printenScore(){

  //Printen van text op LCD Tim
  lcd.setCursor(0,0); //we start writing from the first row first column
  lcd.print( String("uw score: ")+ String(score)); //16 characters poer line
  delay(1000);//Delay used to give a dinamic effect
  lcd.setCursor(0,1);
  lcd.print("Mullersays");

}

int naamInvoeren(){  
  //naam invoeren
  int buttonState1 = digitalRead(button1);
  int buttonState2 = digitalRead(button2);
  lcd.setCursor(0,1);
  lcd.print(String(letter1));
  if (buttonState1 ==  HIGH && letter == 1){
  delay(500);
  letter1++;
}


if (buttonState1 == HIGH && letter == 2){
  lcd.setCursor(1,1);
  lcd.print(String(letter2));
  delay(500);
  letter2++;
  }
  
if (buttonState1 == HIGH && letter == 3){
  lcd.setCursor(2,1);
  lcd.print(String(letter3));
  delay(500);
  letter3++;
  }
  
if (buttonState2 == HIGH){
  delay(500);
  letter++;
  }
if (letter == 3){
  String name = String(letter1) + String(letter2) + String(letter3);
  addScoreToDatabase(score, name);
  }
  
}

//end function
