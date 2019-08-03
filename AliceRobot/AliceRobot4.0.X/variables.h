/* 
 * File:   variables.h
 * Author: AB
 *
 * Created on March 15, 2019, 4:31 PM
 */

#ifndef VARIABLES_H
#define	VARIABLES_H

#include "motors.h"
#include "stdbool.h"
    
#define M_PI 3.14159265358979323846
#define GEAR_RATIO 960//960 b/c 8 pulses per encoder rotation * 120 gearing ratio//120
#define CIRCUMFERENCE 200//63*M_PI
#define ACTIVATION_TOLERANCE 8.0
#define SHUTDOWN_TOLERANCE 5.0

extern volatile bool key_was_pressed; //interrupt for key on keypad being pressed

extern volatile double tickL; //distance travelled by robot given by encoders on RB2 (left motor)
extern volatile double tickR; //distance travelled by robot given by encoders on RB0 (right motor)
extern volatile long int tickLeftTemp;
extern volatile long int tickRightTemp;

extern volatile double distanceL;
extern volatile double distanceR;

extern volatile float masterPower;
extern volatile float slavePower;

extern volatile int travelDirection;

extern int sideTurnedOff;

extern bool moving;
#endif	/* VARIABLES_H */

