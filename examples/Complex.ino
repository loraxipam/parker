// Light some lights as you get closer

/*
 * This sketch uses a TLC59711 to drive the lights rather than direct off the GPIO pins
 */

#include <Ultrasonic.h>
#include "Adafruit_TLC59711.h"
#include <SPI.h>

// The number of boards we're driving?
#define NUM_TLC59711 1

#define LITEME(A) ( depth >= (block * A) ? 65535 : (((depth%block)+1) * (65536/block)-1) )

// Ultrasonic pins
#define ECHO    9
#define TRIG   10

// 59711 pins
#define DATA   11
#define CLOCK  12

// The pin on the 59711 that each LED is using
enum leds { BLUE, GREEN, YELLOW, RED };

// The "block" size of the distance measurement.
// When distance <= block * 4, turn on blue; block * 3, turn on green,
// block * 2 is the best distance with yellow now on and danger within
// block * 1 which turns on the red LED
const static int block = 16;  // Sixteen inch buckets

Adafruit_TLC59711 tlc = Adafruit_TLC59711(NUM_TLC59711, CLOCK, DATA);
Ultrasonic ultrasonic(TRIG, ECHO, 20000UL);

int distance;

// Brighten the LEDs (B, G, Y, R) as distance gets closer

void setup() {
  Serial.begin(115200);
  pinMode(DATA, OUTPUT);
  pinMode(CLOCK, OUTPUT);
  pinMode(TRIG, OUTPUT);
  
  tlc.begin();
  tlc.write();
  analogWrite(13, LOW);
}

void loop() {
  // Pass INC as a parameter to get the distance in inches
  distance = ultrasonic.read(INC);

  // how deep into the range has the object come? Basically
  // the number of inches between OTHER end of the range and
  // the sensor, which will INCREASE as it gets closer
  int depth = distance > (block * 4) ? 0 : ((block * 4) - distance);

  if (distance > (block * 4)) {
    // turn off all the lights
    tlc.setLED(BLUE,   0,0,0);
    tlc.setLED(GREEN,  0,0,0);
    tlc.setLED(YELLOW, 0,0,0);
    tlc.setLED(RED,    0,0,0);
  } else {  // if (distance <= (block * 4))
    // start lighting the blue
    tlc.setLED(BLUE, 0, LITEME(1),0);

    // start lighting the green
    if (distance <= (block * 3)) {
      tlc.setPWM(0, 65535);
      tlc.setLED(GREEN, 0, LITEME(2),0);

      // start lighting the yellow
      if (distance <= (block * 2)) {
      tlc.setPWM(5, 65535);
      tlc.setLED(YELLOW, 0, LITEME(3), 0);

        // start lighting the red
        if (distance <= (block)) {
          tlc.setPWM(6, 65535);
          tlc.setLED(RED, 0, LITEME(4), 0);
        } else {
          tlc.setLED(RED, 0, 0, 0);
        }

      } else {
        tlc.setLED(YELLOW, 0, 0, 0);
      }

      } else {
        tlc.setLED(GREEN, 0, 0, 0);
      }
    }


  tlc.write();

  delay(235);

}
