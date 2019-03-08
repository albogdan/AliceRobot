#include "motors.h"


void DCMotorCenterLeft(){
    CENTER_MOTOR_RIGHT = 0;
    CENTER_MOTOR_LEFT = 1;
}

void DCMotorCenterRight(){
    CENTER_MOTOR_LEFT = 0;
    CENTER_MOTOR_RIGHT = 1;
}

void DCMotorLeftFwd(){
    RIGHT_MOTOR_BKWD = 0; //Turn OFF Bkwd For Right Motor
    LEFT_MOTOR_BKWD = 0; //Turn OFF Bkwd For Left Motor    
    
    LEFT_MOTOR_FWD = 1; //Turn ON Fwd For Left Motor
}
void DCMotorRightFwd(){
    RIGHT_MOTOR_BKWD = 0; //Turn OFF Bkwd For Right Motor
    LEFT_MOTOR_BKWD = 0; //Turn OFF Bkwd For Left Motor    
    
    RIGHT_MOTOR_FWD = 1; //Turn ON Fwd For Right Motor
}
void DCMotorLeftBkwd(){
    RIGHT_MOTOR_FWD = 0; //Turn OFF Fwd For Right Motor
    LEFT_MOTOR_FWD = 0; //Turn OFF Fwd For Left Motor 
    
    LEFT_MOTOR_BKWD = 1;
}
void DCMotorRightBkwd(){
    RIGHT_MOTOR_FWD = 0;  //Turn OFF Fwd For Right Motor
    LEFT_MOTOR_FWD = 0;  //Turn OFF Fwd For Left Motor 
    
    RIGHT_MOTOR_BKWD = 1; //Turn ON Bkwd For Right Motor 
}
void DCMotorAllOff(){
    RIGHT_MOTOR_FWD = 0; //Turn OFF Fwd For Right Motor
    LEFT_MOTOR_FWD = 0; //Turn OFF Fwd For Left Motor 

    RIGHT_MOTOR_BKWD = 0; //Turn OFF Bkwd For Right Motor
    LEFT_MOTOR_BKWD = 0; //Turn OFF Bkwd For Left Motor  
    
    CENTER_MOTOR_LEFT = 0;
    CENTER_MOTOR_RIGHT = 0;
}


void DCMotorLeftRightFwdON(){
    DCMotorLeftFwd();
    DCMotorRightFwd();
}
void DCMotorLeftRightBkwdON(){
    DCMotorLeftBkwd();
    DCMotorRightBkwd();
}

//void DCMotorLeftRightBkwdON(){
//    LATAbits.LATA1 = 1; //Turn ON Bkwd For Right Motor
//    LATAbits.LATA3 = 1; //Turn ON Bkwd For Left Motor   
//
//    LATAbits.LATA0 = 0; //Turn OFF Fwd For Right Motor
//    LATAbits.LATA2 = 0; //Turn OFF Fwd For Left Motor 
//}
//void DCMotorLeftRightOFF(){
//    LATAbits.LATA1 = 0; //Turn OFF Bkwd For Right Motor
//    LATAbits.LATA3 = 0; //Turn OFF Bkwd For Left Motor   
//
//    LATAbits.LATA0 = 0; //Turn OFF Fwd For Right Motor
//    LATAbits.LATA2 = 0; //Turn OFF Fwd For Left Motor 
//}

/*
Middle Motor Fwd	RA4
Middle Motor Bkwd	RA5 / AN4
 */

