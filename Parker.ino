// Garage car parking helper.
// Only scans when the garage door is open, but it would be better while not closed

/*
 * This sketch uses the TLC59711 to drive the lights rather than direct off the GPIO pins.
 * It has two LEDs on each corner rather than just one.
 * It listens for a closed alarm pin which shows the garage door is open, otherwise,
 * turn off all the lights and sleep.
 */

#include <LowPower.h>
#include <Ultrasonic.h>
#include "Adafruit_TLC59711.h"
#include <SPI.h>

// The number of TLC59711 boards we're driving
#define NUM_TLC59711 1

// Max brightness on the TLC59711
#define MAX 65535

// Depth bucket calculator
#define LITEME(A) ( depth >= (block * A) ? MAX : (((depth%block)+1) * ((MAX+1)/block)-1) )

// Ultrasonic pins
#define ECHO    9
#define TRIG   10

// 59711 pins
#define DATA   11
#define CLOCK  12

// tilt sensor pin
#define TILT   13

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
  pinMode(ECHO,   INPUT);
  pinMode(TRIG,  OUTPUT);
  pinMode(DATA,  OUTPUT);
  pinMode(CLOCK, OUTPUT);
  pinMode(TILT,  INPUT_PULLUP);   // This might need changing

  tlc.begin();
  tlc.write();
}

void loop() {
  while (doorOpen(TILT)) {
  // Pass INC as a parameter to get the distance in inches
  distance = ultrasonic.read(INC);

  // how deep into the range has the object come? Basically
  // the number of inches between the OTHER end of the range and
  // the sensor, which will INCREASE as it gets closer
  int depth = distance > (block * 4) ? 0 : ((block * 4) - distance);

  Serial.print(depth);Serial.print("\t");
  Serial.println(distance);


  if (distance > (block * 4)) {
    // turn off all the lights
    turnOffAllLights();
  } else {  // if (distance <= (block * 4))
    // start lighting the blue
    tlc.setLED(BLUE, LITEME(1), LITEME(1), 0);

    // start lighting the green
    if (distance <= (block * 3)) {
      tlc.setLED(GREEN, 0, LITEME(2), LITEME(2));

      // start lighting the yellow
      if (distance <= (block * 2)) {
        tlc.setLED(YELLOW, LITEME(3), LITEME(3), 0);

        // start lighting the red
        if (distance <= (block)) {
          tlc.setLED(RED, 0, LITEME(4), LITEME(4));
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

    delay(75);
  }

  // This is where it should sleep if the door has been closed for ages
  turnOffAllLights();
  LowPower.idle(SLEEP_8S, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF, 
                SPI_ON, USART0_OFF, TWI_ON);

  // Somehow, too, you'd want to have a case where the car is there and
  // the door is up but it's been open for an hour; what then?
  
}

void turnOffAllLights(){
    tlc.setLED(BLUE,   0,0,0);
    tlc.setLED(GREEN,  0,0,0);
    tlc.setLED(YELLOW, 0,0,0);
    tlc.setLED(RED,    0,0,0);
    tlc.write();
}

// My tilt sensor returns 1 when open and 0 when closed
bool doorOpen(int pin){
  return !bool(digitalRead(pin));
}
