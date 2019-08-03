#include "motors.h"


void DCMotorLeftOFF(){ //Turn off left motor
    LEFT_MOTOR_FWD = 0;
    LEFT_MOTOR_BKWD = 0;
}
void DCMotorRightOFF(){ //Turn off right motor
    RIGHT_MOTOR_FWD = 0;
    RIGHT_MOTOR_BKWD = 0;
}

void DCMotorCenterLeft(){ //Move center motor to the left
    CENTER_MOTOR_RIGHT = 0;
    CENTER_MOTOR_LEFT = 1;
}

void DCMotorCenterRight(){ //Move center motor to the right
    CENTER_MOTOR_LEFT = 0;
    CENTER_MOTOR_RIGHT = 1;
}
void DCMotorLeftFwd(){ //Move left motor fwd
    RIGHT_MOTOR_BKWD = 0; //Turn OFF Bkwd For Right Motor
    LEFT_MOTOR_BKWD = 0; //Turn OFF Bkwd For Left Motor    
    
    LEFT_MOTOR_FWD = 1; //Turn ON Fwd For Left Motor
}
void DCMotorRightFwd(){ //Move right motor fwd
    RIGHT_MOTOR_BKWD = 0; //Turn OFF Bkwd For Right Motor
    LEFT_MOTOR_BKWD = 0; //Turn OFF Bkwd For Left Motor    
    
    RIGHT_MOTOR_FWD = 1; //Turn ON Fwd For Right Motor
}
void DCMotorLeftBkwd(){ //Move left motor bkwd
    RIGHT_MOTOR_FWD = 0; //Turn OFF Fwd For Right Motor
    LEFT_MOTOR_FWD = 0; //Turn OFF Fwd For Left Motor 
    
    LEFT_MOTOR_BKWD = 1;
}
void DCMotorRightBkwd(){ ///Move right motor bkwd
    RIGHT_MOTOR_FWD = 0;  //Turn OFF Fwd For Right Motor
    LEFT_MOTOR_FWD = 0;  //Turn OFF Fwd For Left Motor 
    
    RIGHT_MOTOR_BKWD = 1; //Turn ON Bkwd For Right Motor 
}
void DCMotorAllOff(){ //Turn off all motors
    RIGHT_MOTOR_FWD = 0; //Turn OFF Fwd For Right Motor
    LEFT_MOTOR_FWD = 0; //Turn OFF Fwd For Left Motor 

    RIGHT_MOTOR_BKWD = 0; //Turn OFF Bkwd For Right Motor
    LEFT_MOTOR_BKWD = 0; //Turn OFF Bkwd For Left Motor  
    
    CENTER_MOTOR_LEFT = 0;
    CENTER_MOTOR_RIGHT = 0;
}
void DCMotorLeftRightFwdON(){ //Move left and right motors fwd
    DCMotorLeftFwd();
    DCMotorRightFwd();
}
void DCMotorLeftRightBkwdON(){ //Move left and right motors bkwd
    DCMotorLeftBkwd();
    DCMotorRightBkwd();
}