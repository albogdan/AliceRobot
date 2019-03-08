#include <AccelStepper.h>
#define HALFSTEP 8

/* *****INFORMATION*****
 *  https://42bots.com/tutorials/28byj-48-stepper-motor-with-uln2003-driver-and-arduino-uno/
 * 4076 steps per FULL revolution
 * 2038 steps per HALF revolution
 * Original position at boot is set as zero
 * Moving a half revolution moves to 180 degrees, moving to half again remains the same
 * stepper1.currentPosition() is the current step value
 */

 #define QUARTER_REVOLUTION 1019
 #define HALF_REVOLUTION 2038
 #define FULL_REVOLUTION 4076

// Motor pin definitions
#define motorPin1  3     // IN1 on the ULN2003 driver 1
#define motorPin2  4     // IN2 on the ULN2003 driver 1
#define motorPin3  5     // IN3 on the ULN2003 driver 1
#define motorPin4  6     // IN4 on the ULN2003 driver 1

// Initialize with pin sequence IN1-IN3-IN2-IN4 for using the AccelStepper with 28BYJ-48
AccelStepper stepper1(HALFSTEP, motorPin1, motorPin3, motorPin2, motorPin4);

void setup() {
  stepper1.setMaxSpeed(2000.0);
  stepper1.setAcceleration(100.0);
  stepper1.setSpeed(700);
  stepper1.moveTo(HALF_REVOLUTION);

}//--(end setup )---

void loop() {

  //Change direction when the stepper reaches the target position
  if (stepper1.distanceToGo() == 0) {
    stepper1.moveTo(0);
  }
  stepper1.run();
}
