/* 
 * File:   motors.h
 * Author: AB
 *
 * Created on February 25, 2019, 7:28 PM
 */

#ifndef MOTORS_H
#define	MOTORS_H

#include <xc.h>

#define RIGHT_MOTOR_FWD LATAbits.LATA0 //PWM ON RC2
#define RIGHT_MOTOR_BKWD LATAbits.LATA1 //PWM ON RC2
    
#define LEFT_MOTOR_FWD LATAbits.LATA2 //PWM ON RC1
#define LEFT_MOTOR_BKWD LATAbits.LATA3 //PWM ON RC1
    
#define CENTER_MOTOR_LEFT LATAbits.LATA4
#define CENTER_MOTOR_RIGHT LATAbits.LATA5    

#define DIRECTION_STATIONARY 0
#define DIRECTION_FORWARD 1
#define DIRECTION_BACKWARD 2
#define DIRECTION_LEFT 3
#define DIRECTION_RIGHT 4
    
#define STOPPED_NONE 0
#define STOPPED_LEFT 1
#define STOPPED_RIGHT 2
 
void DCMotorLeftOFF();
void DCMotorRightOFF();
    
void DCMotorCenterRight();
void DCMotorCenterLeft();

void DCMotorLeftFwd();
void DCMotorRightFwd();
void DCMotorLeftBkwd();
void DCMotorRightBkwd();
void DCMotorAllOff();

void DCMotorLeftRightFwdON();
void DCMotorLeftRightBkwdON();
#endif	/* MOTORS_H */

