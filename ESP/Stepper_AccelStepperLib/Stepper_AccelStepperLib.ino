#include <AccelStepper.h>
#define HALFSTEP 8
#define FULLSTEP 4
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
 #define FIRST_TIRE_DEPLOY_7 -1950
 #define REGULAR_TIRE_DEPLOY_7 -1880

 #define FIRST_TIRE_DEPLOY_8 -775//-1550
 #define REGULAR_TIRE_DEPLOY_8 -850//-1700
 
// Motor pin definitions ESP
#define motorPin1  D7     // IN1 on the ULN2003 driver 1
#define motorPin2  D6     // IN2 on the ULN2003 driver 1
#define motorPin3  D5     // IN3 on the ULN2003 driver 1
#define motorPin4  D4     // IN4 on the ULN2003 driver 1

#define motorPin5 D0     //IN1
#define motorPin6 D3    //IN2
#define motorPin7 D8     //IN3
#define motorPin8 10     //IN4 //GPIO 10 SEEMS TO WORK??

// Motor pin definitions UNO
//#define motorPin1  3     // IN1 on the ULN2003 driver 1
//#define motorPin2  4     // IN2 on the ULN2003 driver 1
//#define motorPin3  5     // IN3 on the ULN2003 driver 1
//#define motorPin4  6     // IN4 on the ULN2003 driver 1

// Initialize with pin sequence IN1-IN3-IN2-IN4 for using the AccelStepper with 28BYJ-48
AccelStepper stepper1(FULLSTEP, motorPin1, motorPin3, motorPin2, motorPin4);
//AccelStepper stepperConveyor(HALFSTEP, motorPin1, motorPin3, motorPin2, motorPin4);
//AccelStepper stepper1(HALFSTEP, motorPin5, motorPin7, motorPin6, motorPin8);

int tireNumber = 0;
void setup() {
  stepper1.setMaxSpeed(2000.0);
  stepper1.setAcceleration(100.0);
  stepper1.setSpeed(1950);
//  stepper1.moveTo(4*FULL_REVOLUTION);
  stepper1.moveTo(FIRST_TIRE_DEPLOY_8);
}//--(end setup )---

void loop() {
//  stepper1.moveTo(FIRST_TIRE_DEPLOY);
//  while(1){
//      if (stepper1.distanceToGo() == 0) {
//          break;
//      }
//      stepper1.run();
//  }
//  delay(1000);
//  stepper1.moveTo(REGULAR_TIRE_DEPLOY);
//  while(1){
//      if (stepper1.distanceToGo() == 0) {
//          break;
//      }
//      stepper1.run();
//  }
//  delay(1000);
  if(stepper1.distanceToGo() == 0 && tireNumber == 0){
    long int toMove = FIRST_TIRE_DEPLOY_8 + REGULAR_TIRE_DEPLOY_8;
    stepper1.moveTo(toMove);
    tireNumber +=1;
    delay(1000);
  }
  if(stepper1.distanceToGo() == 0 && tireNumber == 1){
    long int toMove = FIRST_TIRE_DEPLOY_8 + 2*REGULAR_TIRE_DEPLOY_8;
    stepper1.moveTo(toMove);
    tireNumber +=1;
    delay(1000);
  }
  if(stepper1.distanceToGo() == 0 && tireNumber == 2){
    long int toMove = FIRST_TIRE_DEPLOY_8 + 3*REGULAR_TIRE_DEPLOY_8;
    stepper1.moveTo(toMove);
    tireNumber +=1;
    delay(1000);
  }
  if(stepper1.distanceToGo() == 0 && tireNumber == 3){
    long int toMove = FIRST_TIRE_DEPLOY_8 + 4*REGULAR_TIRE_DEPLOY_8;
    stepper1.moveTo(toMove);
    tireNumber +=1;
    delay(1000);
  }
  if(stepper1.distanceToGo() == 0 && tireNumber == 4){
    long int toMove = FIRST_TIRE_DEPLOY_8 + 5*REGULAR_TIRE_DEPLOY_8;
    stepper1.moveTo(toMove);
    tireNumber +=1;
    delay(1000);
  }
    if(stepper1.distanceToGo() == 0 && tireNumber == 5){
    long int toMove = FIRST_TIRE_DEPLOY_8 + 6*REGULAR_TIRE_DEPLOY_8;
    stepper1.moveTo(toMove);
    tireNumber +=1;
    delay(1000);
  }
    if(stepper1.distanceToGo() == 0 && tireNumber == 6){
    long int toMove = FIRST_TIRE_DEPLOY_8 + 7*REGULAR_TIRE_DEPLOY_8;
    stepper1.moveTo(toMove);
    tireNumber +=1;
    delay(1000);
  }
  
  stepper1.run();

  
}
