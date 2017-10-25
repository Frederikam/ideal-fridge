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
#include "Ultrasonic.h"
Ultrasonic ultrasonic(2); // Pins 2-3
const int outputPin = A5;

void setup() {
  Serial.begin(9600);
  pinMode(outputPin, OUTPUT);
}

void loop() {
    int dist = getDist();
    analogWrite(outputPin, dist * 10);
    Serial.println(dist, DEC);
}
 /*
 * If called too frequently the Ultrasonic class will return 0. 
 */
int getDist() {
  return (getDist0() + getDist0() + getDist0() + getDist0() + getDist0()) / 5;
}

int getDist0() {
  while (true) {
    int dist = ultrasonic.MeasureInCentimeters();
    if (dist != 0 && dist < 100) {
      return dist;
    }
  }
}