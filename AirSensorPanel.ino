#include "SparkFun_SGP30_Arduino_Library.h"
#include <Wire.h>
#include "FastLED.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"
#include <avr/wdt.h>

#define NUM_LEDS_FIRSTRed 4
#define NUM_LEDS_FIRSTBlue 4
#define NUM_LEDS_SECOND 2
#define NUM_LEDS_THIRD 12
#define COLOR_ORDER RGB
#define LED_TYPE WS2811
#define ledBootPin 13
#define hueUp A2
#define hueDown A1

const int slidePin = 2;
const int potDialPin = A0;
int potDialRaw = 0;
int potDial = 0;
int potDialLast = 0;
int levelVOC = 0;
int levelCO2 = 0;
int potPosition = 0;
int buttonState = 0;
int hueUpPressed =0;
int hueDownPressed =0;
uint8_t max_bright = 128;   

boolean buttonAlreadyOff = false;
boolean buttonAlreadyOn = false;
boolean redHue = false;
boolean orangeHue = false;
boolean yellowHue = false;
boolean greenHue = false;
boolean tealHue = false;
boolean blueHue = false;
boolean purpleHue = false;

SGP30 mySensor;

SoftwareSerial mySoftwareSerial(3, 4); // RX, TX
DFRobotDFPlayerMini myDFPlayer;

CRGB firstRedLeds[NUM_LEDS_FIRSTRed];
CRGB firstBlueLeds[NUM_LEDS_FIRSTBlue];
CRGB secondLeds[NUM_LEDS_SECOND];
CRGB thirdLeds[NUM_LEDS_THIRD];


void setup() {

  mySoftwareSerial.begin(9600);
  Wire.begin();
  if (mySensor.begin() == false) {
    while (1);
  }

  mySensor.initAirQuality();

  pinMode(ledBootPin, OUTPUT);
  pinMode(slidePin, INPUT_PULLUP);
  digitalWrite(ledBootPin,LOW);
  pinMode(hueUp, INPUT);
  pinMode(hueDown, INPUT);

  if (!myDFPlayer.begin(mySoftwareSerial)) {  
    while (true);
  }

  myDFPlayer.volume(30);  //Set volume value. From 0 to 30
  
  FastLED.addLeds<NEOPIXEL, 9>(firstRedLeds, NUM_LEDS_FIRSTRed);
  FastLED.addLeds<NEOPIXEL, 8>(firstBlueLeds, NUM_LEDS_FIRSTBlue);
  FastLED.addLeds<NEOPIXEL, 5>(secondLeds, NUM_LEDS_SECOND);
  FastLED.addLeds<NEOPIXEL, 6>(thirdLeds, NUM_LEDS_THIRD);

  FastLED.setBrightness(max_bright);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 500);

  fill_solid(firstRedLeds, NUM_LEDS_FIRSTRed, CRGB::Black);
  fill_solid(firstBlueLeds, NUM_LEDS_FIRSTBlue, CRGB::Black);
  fill_solid(secondLeds, NUM_LEDS_SECOND, CRGB::Black);
  fill_solid(thirdLeds, NUM_LEDS_THIRD, CRGB::Black);
  FastLED.show();


  potDialLast = analogRead(A0);
  potDialRaw = analogRead(A0);

  wdt_enable(WDTO_8S);

  delay(1000);

}

void loop() {

 
  
  buttonState = digitalRead(slidePin);

  if ((buttonState == LOW)&&(buttonAlreadyOff == false)) {
    buttonWait(2);
  }
  
  if ((buttonState == HIGH)&& (buttonAlreadyOn == false)) {
    bootUp();
  }

  wdt_reset();

  if (buttonAlreadyOff == true) {
    return;
  }

  hueUpPressed = digitalRead(hueUp);
  hueDownPressed = digitalRead(hueDown);

  if (hueUpPressed == HIGH) {
    displayColorUp();
  }

  if (hueDownPressed == HIGH) {
    displayColorDown();
  }

  potDialRaw = analogRead(A0);

  potDial = map(potDialRaw, 0, 1023, 0, 9);

  if (potDial!=potDialLast) {
    potReadFunction();
    potDialLast = potDial;
    return;
}
    else {

  sensorReadFunction();
    }
}


void buttonWait(int slidePin){
  int buttonState = 0;
  while(1){
    buttonState = digitalRead(slidePin);
    wdt_reset();
    if (buttonState == LOW) {
      myDFPlayer.play(5);
    fill_solid(firstRedLeds, NUM_LEDS_FIRSTRed, CRGB::Black);
    fill_solid(firstBlueLeds, NUM_LEDS_FIRSTBlue, CRGB::Black);
    fill_solid(secondLeds, NUM_LEDS_SECOND, CRGB::Black);
    fill_solid(thirdLeds, NUM_LEDS_THIRD, CRGB::Black);
    FastLED.show();
    digitalWrite(ledBootPin, LOW);
    buttonAlreadyOff = true;
    buttonAlreadyOn = false;
      return;
    }
  }
}
  
void bootUp() {

  digitalWrite(ledBootPin, HIGH);
  myDFPlayer.play(1);
  delay(1500);
  
  fill_solid(firstRedLeds, NUM_LEDS_FIRSTRed, CRGB::Red);
  fill_solid(firstBlueLeds, NUM_LEDS_FIRSTBlue, CRGB::Blue);
  fill_solid(secondLeds, NUM_LEDS_SECOND, CRGB(10,0,0));
  FastLED.show();
  delay(1500);
  buttonAlreadyOn = true;
  buttonAlreadyOff = false;
  redHue = true;
  orangeHue = false;
  yellowHue = false;
  greenHue = false;
  tealHue = false;
  blueHue = false;
  purpleHue = false;
}

void displayColorDown() {
  
  if (redHue == true) {
    redHue = false;
    orangeHue = true;
    fill_solid(secondLeds, NUM_LEDS_SECOND, CRGB::DarkOrange);
    myDFPlayer.play(3);
  }
  else if (orangeHue == true) {
    orangeHue = false;
    yellowHue = true;
    fill_solid(secondLeds, NUM_LEDS_SECOND, CRGB::Gold);
    myDFPlayer.play(3);
  }
  else if (yellowHue == true) {
    yellowHue = false;
    greenHue = true;
    fill_solid(secondLeds, NUM_LEDS_SECOND, CRGB::Green);
    myDFPlayer.play(3);
  }
  else if (greenHue == true) {
    greenHue = false;
    tealHue = true;
    fill_solid(secondLeds, NUM_LEDS_SECOND, CRGB::Teal);
    myDFPlayer.play(3);
  }
  else if (tealHue == true) {
    tealHue = false;
    blueHue = true;
    fill_solid(secondLeds, NUM_LEDS_SECOND, CRGB::Blue);
    myDFPlayer.play(3);
  }
  else if (blueHue == true) {
    blueHue = false;
    purpleHue = true;
    fill_solid(secondLeds, NUM_LEDS_SECOND, CRGB::Purple);
    myDFPlayer.play(3);
  }
  FastLED.show();
  
}

void displayColorUp() {
  
  if (purpleHue == true) {
    purpleHue = false;
    blueHue = true;
    fill_solid(secondLeds, NUM_LEDS_SECOND, CRGB::Blue);
    myDFPlayer.play(3);
  }
  else if (blueHue == true) {
    blueHue = false;
    tealHue = true;
    fill_solid(secondLeds, NUM_LEDS_SECOND, CRGB::Teal);
    myDFPlayer.play(3);
  }
  else if (tealHue == true) {
    tealHue = false;
    greenHue = true;
    fill_solid(secondLeds, NUM_LEDS_SECOND, CRGB::Green);
    myDFPlayer.play(3);
  }
  else if (greenHue == true) {
    greenHue = false;
    yellowHue = true;
    fill_solid(secondLeds, NUM_LEDS_SECOND, CRGB::Gold);
    myDFPlayer.play(3);
  }
  else if (yellowHue == true) {
    yellowHue = false;
    orangeHue = true;
    fill_solid(secondLeds, NUM_LEDS_SECOND, CRGB::DarkOrange);
    myDFPlayer.play(3);
  }
  else if (orangeHue == true) {
    orangeHue = false;
    redHue = true;
    fill_solid(secondLeds, NUM_LEDS_SECOND, CRGB::Red);
    myDFPlayer.play(3);
  }
  FastLED.show();
}

void sensorReadFunction() {

  delay(500);
  mySensor.measureAirQuality();

  levelVOC = mySensor.TVOC;
  levelCO2 = mySensor.CO2/4;

  if (potDial == 0) {
    fill_solid(thirdLeds, NUM_LEDS_THIRD, CRGB(levelVOC,255-levelVOC,0));
    FastLED.show();
  }
  else if (potDial == 8 ) {
    fill_solid(thirdLeds, NUM_LEDS_THIRD, CRGB(levelCO2,255-levelCO2,0));
    FastLED.show();
  }
  else {
    fill_solid(thirdLeds, NUM_LEDS_THIRD, CRGB(levelVOC,255-levelCO2,0));
    FastLED.show();
  }


}

void potReadFunction() {

  
  if (potDial == 0) {
    fill_solid(firstBlueLeds, NUM_LEDS_FIRSTBlue, CRGB::Blue);
    fill_solid(firstRedLeds, NUM_LEDS_FIRSTRed, CRGB::Black);
    FastLED.show();
    myDFPlayer.play(2);
  }
  if (potDial ==1) {
    fill_solid(firstBlueLeds, 2, CRGB::Blue);
    firstBlueLeds[3] = CRGB::Black;
    fill_solid(firstRedLeds, NUM_LEDS_FIRSTRed, CRGB::Black);
    FastLED.show();
    myDFPlayer.play(2);
  }
  if (potDial == 2) {
    fill_solid(firstBlueLeds, 1, CRGB::Blue);
    fill_solid(firstRedLeds, NUM_LEDS_FIRSTRed, CRGB::Black);
    firstBlueLeds[3] = CRGB::Black;
    firstBlueLeds[2] = CRGB:: Black;
    FastLED.show();
    myDFPlayer.play(2);
  }
  if (potDial == 3) {
    fill_solid(firstBlueLeds, 0, CRGB::Blue);
    fill_solid(firstRedLeds, NUM_LEDS_FIRSTRed, CRGB::Black);
    firstBlueLeds[3] = CRGB::Black;
    firstBlueLeds[2] = CRGB:: Black;
    firstBlueLeds[1] = CRGB::Black;
    FastLED.show();
    myDFPlayer.play(2);
  }
  if (potDial == 4) {
    firstBlueLeds[3] = CRGB::Black;
    firstBlueLeds[2] = CRGB:: Black;
    firstBlueLeds[1] = CRGB::Black;
    firstBlueLeds[0] = CRGB::Blue;
    firstRedLeds[3] = CRGB::Black;
    firstRedLeds[2] = CRGB:: Black;
    firstRedLeds[1] = CRGB::Black;
    firstRedLeds[0] = CRGB::Red;
    FastLED.show();
    myDFPlayer.play(2);
  }
  if (potDial ==5) {
    fill_solid(firstRedLeds, 0, CRGB::Red);
    fill_solid(firstBlueLeds, NUM_LEDS_FIRSTRed, CRGB::Black);
    firstRedLeds[3] = CRGB::Black;
    firstRedLeds[2] = CRGB:: Black;
    firstRedLeds[1] = CRGB::Black;
    FastLED.show();
    myDFPlayer.play(2);
  }
  if (potDial ==6) {
    fill_solid(firstRedLeds, 1, CRGB::Red);
    fill_solid(firstBlueLeds, NUM_LEDS_FIRSTRed, CRGB::Black);
    firstRedLeds[3] = CRGB::Black;
    firstRedLeds[2] = CRGB:: Black;
    FastLED.show();
    myDFPlayer.play(2);
  }
  if (potDial ==7) {
    fill_solid(firstBlueLeds, NUM_LEDS_FIRSTBlue, CRGB::Black);
    fill_solid(firstRedLeds, 2, CRGB::Red);
    firstRedLeds[3] = CRGB::Black;
    FastLED.show();
    myDFPlayer.play(2);
  }
  if (potDial ==8) {
    fill_solid(firstBlueLeds, NUM_LEDS_FIRSTBlue, CRGB::Black);
    fill_solid(firstRedLeds, NUM_LEDS_FIRSTRed, CRGB::Red);
    FastLED.show();
    myDFPlayer.play(2);
  }
}
