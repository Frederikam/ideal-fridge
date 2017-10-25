/*

This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <https://unlicense.org>

*/

#include <Wire.h>
#include <SerialLCD.h>
#include <SoftwareSerial.h>
#include "Ultrasonic.h"

const int conveyorLengthCm = 110;
const int conveyorStep = 5; // Distance to move at a time
Ultrasonic ultrasonic(2); // Pins 2-3
SerialLCD lcd(4, 5);
int buttons[] = {
    6,  // Increment
    7,  // Decrement
    8   // Change unit
};
const int relayPin = 9;

unsigned long units[] = {
    60000,         // Minute
    3600000,       // Hour
    86400000,      // Day
    604800000,     // Week
    2592000000,    // Month (approx)
};

unsigned long lastMove = 0;
int unit = 0;
int duration = 30;

void setup() {
  Serial.begin(9600);
  pinMode(buttons[0], INPUT);
  pinMode(buttons[1], INPUT);
  pinMode(buttons[2], INPUT);
  pinMode(relayPin, OUTPUT);
  lcd.begin();
  lcd.backlight();
}

void loop() {
  if (checkButton(0)) {
    duration++;
  } else if (checkButton(1)) {
    duration--;
  }

  if (checkButton(2)){
    unit++;
  }

  Serial.println(duration, DEC);

  drawLcd();

  // Ensure bounds
  duration = max(0, duration);
  duration = min(60, duration);
  if(unit > 4) {
    unit = 0;
  } else if (unit < 0) {
    unit = 4;
  }

  if (getDist() > conveyorLengthCm + 5) return; // Nothing on conveyor

  if (millis() > nextMoveTime()){
    move5cm();
  }
}

void move5cm() {
  lastMove = millis();
  int startDist = getDist();
  while(getDist() - startDist < 5) {
    digitalWrite(relayPin, HIGH);
    delay(10);
  }
  digitalWrite(relayPin, LOW);
}

int nextMoveTime() {
    int steps = conveyorLengthCm / conveyorStep;
    return lastMove + (units[unit] * duration) * steps;
}

int lastButtonTimes[] = {0, 0, 0};
int threshold = 500;

bool checkButton(int btn) {
    if (digitalRead(buttons[btn]) == HIGH
            && millis() - lastButtonTimes[btn] > threshold) {
        lastButtonTimes[btn] = millis();
        return true;
    }
    return false;
}

void drawLcd() {
  lcd.clear();
  lcd.print(duration, DEC);
  lcd.print(" ");

  switch(unit) {
    case 0: lcd.print("Minute"); break;
    case 1: lcd.print("Hour"); break;
    case 2: lcd.print("Day"); break;
    case 3: lcd.print("Week"); break;
    case 4: lcd.print("Month"); break;
  }
  
  // Singular or plural?
  if (duration != 1) { 
    lcd.print("s");
  }
}

/*
 * If called too frequently the Ultrasonic class will return 0. 
 */
int getDist() {
  return (getDist0() + getDist0()) / 2;
}

int getDist0() {
  while (true) {
    int dist = ultrasonic.MeasureInCentimeters();
    if (dist != 0 && dist < 100) {
      return dist;
    }
  }
}

