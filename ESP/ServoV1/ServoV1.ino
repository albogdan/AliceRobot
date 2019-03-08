#include <Servo.h>

Servo myServo; //Create servo object to control a servo
void setup() {
  myServo.attach(9); //attaches servo to pin 9 on servo object
}

void loop() {
  myServo.write(180);
  delay(5000);
}
