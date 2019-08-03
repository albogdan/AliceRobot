#include <Wire.h>
#include "Adafruit_VL53L0X.h"
#include <AccelStepper.h>

Adafruit_VL53L0X lox = Adafruit_VL53L0X();

bool stringComplete = false;
String inputString = "";

#define HALFSTEP 8
#define motorPin5 D0     //IN1
#define motorPin6 D3    //IN2
#define motorPin7 D8     //IN3
#define motorPin8 10     //IN4 //GPIO 10 SEEMS TO WORK??

#define HOME 0
#define TOP -2700
#define BOTTOM -5400

AccelStepper stepperSensor(HALFSTEP, motorPin5, motorPin7, motorPin6, motorPin8);



void setup() {
  stepperSensor.setMaxSpeed(2000.0);
  stepperSensor.setAcceleration(100.0);
  stepperSensor.setSpeed(1950);
//  stepper1.moveTo(4*FULL_REVOLUTION);
  stepperSensor.moveTo(TOP);
}

int tireNumber = 0;
void loop() {
    if(stepperSensor.distanceToGo() == 0 && tireNumber == 0){
      long int toMove = BOTTOM;
      stepperSensor.moveTo(toMove);
      tireNumber +=1;
      delay(2000);
    }
    if(stepperSensor.distanceToGo() == 0 && tireNumber == 1){
      long int toMove = HOME;
      stepperSensor.moveTo(toMove);
      tireNumber +=1;
      delay(2000);
    }

    stepperSensor.run();
    yield();
}
