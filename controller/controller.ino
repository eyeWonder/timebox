#include <JeeLib.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include "constants.h"

ISR(WDT_vect) { Sleepy::watchdogEvent(); }
LiquidCrystal_I2C lcd(0x27,16,2);  

#define DEBUG

int state = STATE_MENU_DETAILS;

int oldPotiSegment  = 0;
int potiSegment     = oldPotiSegment;
                    
// ---------------------------

int optInterval     =       1;
int optIterations   =     100;

const int valuesInterval[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

// ---------------------------

void setup() {
  
  Serial.begin(9600);
  while (!Serial) {
    ; 
  }
  
  Serial.println("init");
  initButtons();
  
  #ifdef USE_DISPLAY
    digitalWrite(PIN_DISPLAY_EN, LOW);
    delay(100);
    lcd.init();                      // initialize the lcd 
    lcd.backlight();
    lcd.setCursor(0,0);
    lcd.print("TIMEBOXGO");
    lcd.setCursor(0,1);
    lcd.print("version: ");
    lcd.setCursor(9,1);
    lcd.print(VERSION);
    
    wait(1);
    Serial.println("display initialized");
  #endif
}


void loop() {

  #ifdef DEBUG 
    Serial.println(state);
    delay(10);
  #endif

  if (state % 10 == 0) {
    potiSegment = getPotiPosition(6);
    if (potiSegment != oldPotiSegment) {
      state = potiSegment * 10 + 1;
    }

    if (buttonPressed()) {
      state = potiSegment * 10 + 2;
    }
  }

  switch(state) {

    case STATE_MENU_DETAILS_DRAW:
      lcd.clear();
      lcd.setCursor(10,0);
      lcd.print("C1:");
      lcd.setCursor(15,0);
      lcd.print("v");
      lcd.setCursor(10,1);
      lcd.print("C2:");
      lcd.setCursor(15,1);
      lcd.print("v");

      state--;
    break;
        
    case STATE_MENU_INTERVAL_DRAW: // +1
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("INTERVAL");
      lcd.setCursor(10,0);
      lcd.print(optInterval);
      state--;
    break;

    case STATE_MENU_INTERVAL_SELECTED: // +2
      potiSegment = getPotiPosition(sizeof(valuesInterval));
      if (potiSegment != oldPotiSegment) {
        //lcd.clear();
        
        lcd.setCursor(0,1);
        lcd.print(valuesInterval[potiSegment]);

        // total Time
        lcd.setCursor(8,1);
        lcd.print("T:");
        lcd.setCursor(10,1);
        lcd.print(String((valuesInterval[potiSegment] * optIterations) / 60) + "h");

        oldPotiSegment = potiSegment;
      }
    
      if (buttonPressed()) {
        // saveValue...
        state = STATE_MENU_INTERVAL;
      }
      
    break;
      
    default:
      if (state % 10 == 1) {
        state -= 1;
        return;
      }

      if (state % 10 == 2) {
        state -= 2;
        return;
      }
      
      //Serial.println("ERR: no valid state");
  }

  wait(1);
}

void initButtons() {
  pinMode(PIN_DISPLAY_EN,     OUTPUT);  
  pinMode(PIN_PHOTOCELL_EN,   OUTPUT);  
  
  pinMode(PIN_PHOTOCELL,      INPUT);  
  pinMode(PIN_POTENTIOMETER,  INPUT);  
  pinMode(PIN_PUSHBUTTON,     INPUT);    
}

void takePicture() {
  digitalWrite(PIN_CAMERA_HIGHSIDE, HIGH);
  
  wait(PRE_TRIGGER_WAIT);
  
  digitalWrite(PIN_CAMERA_SHUTTER, HIGH);
  delay(100);
  digitalWrite(PIN_CAMERA_SHUTTER, LOW);

  wait(POST_TRIGGER_WAIT);

  digitalWrite(PIN_CAMERA_HIGHSIDE, LOW);
}

int getPotiPosition(int numberOfSegments) {
  int segmentSize = 1024 / numberOfSegments;
  
  //return analogRead(PIN_POTENTIOMETER) / segmentSize;

  return 4;
}

boolean buttonPressed() {
  for (int i=0; i<3; i++) {
    if (!digitalRead(PIN_PUSHBUTTON)) {
      return false;
    }
  }

  return true;
}

void wait(byte seconds) {
    for (byte i = 0; i < seconds; ++i) {
      Sleepy::loseSomeTime(1000);
  }
}
