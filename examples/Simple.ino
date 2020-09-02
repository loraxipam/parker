// Light some lights when you are close enough

#include <Ultrasonic.h>

Ultrasonic ultrasonic(10, 9, 20000UL);
int distance;

//// LEDs (B, G, Y, R) on 2, 3, 4, 5 as distance gets closer

void setup() {
  Serial.begin(115200);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
}

void loop() {
  // Pass INC as a parameter to get the distance in inches
  distance = ultrasonic.read(INC);
  
  Serial.print("Distance in inches: ");
  Serial.println(distance);
  if (distance > 50) {
    // turn off all the lights
    digitalWrite(2, LOW);
    digitalWrite(3, LOW);
    digitalWrite(4, LOW);
    digitalWrite(5, LOW);
  }

  if (distance <= 50) {
    digitalWrite(2, HIGH);
  if (distance <= 40) {
    // turn off all the lights
    digitalWrite(3, HIGH);
  if (distance <= 30) {
    // turn off all the lights
    digitalWrite(4, HIGH);
  if (distance <= 20) {
    // turn off all the lights
    digitalWrite(5, HIGH);
  } else digitalWrite(5, LOW);
  } else digitalWrite(4, LOW);
  } else digitalWrite(3, LOW);
  } else digitalWrite(2, LOW);

  delay(250);

}
