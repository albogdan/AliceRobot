#include "variables.h"


void initGlobals(){
    volatile bool key_was_pressed = false; //interrupt for key on keypad being pressed


    volatile double tickL = 0.0; //distance travelled by robot given by encoders on RB2 (left motor)
    volatile double tickR = 0.0; //distance travelled by robot given by encoders on RB0 (right motor)

    volatile double distanceL = 0.0;
    volatile double distanceR = 0.0;

    volatile int travelDirection = DIRECTION_STATIONARY;

    int sideTurnedOff = STOPPED_NONE;

     bool moving = false;

}