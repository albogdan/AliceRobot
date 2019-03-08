/* 
 * File:   motors.h
 * Author: AB
 *
 * Created on February 25, 2019, 7:28 PM
 */

#ifndef MOTORS_H
#define	MOTORS_H

#ifdef	__cplusplus
extern "C" {
#endif
#include <xc.h>

#define RIGHT_MOTOR_FWD LATAbits.LATA0
#define RIGHT_MOTOR_BKWD LATAbits.LATA1
#define LEFT_MOTOR_FWD LATAbits.LATA2
#define LEFT_MOTOR_BKWD LATAbits.LATA3    
#define CENTER_MOTOR_LEFT LATAbits.LATA4
#define CENTER_MOTOR_RIGHT LATAbits.LATA5    

    
    
void DCMotorCenterRight();
void DCMotorCenterLeft();

    
void DCMotorLeftFwd();
void DCMotorRightFwd();
void DCMotorLeftBkwd();
void DCMotorRightBkwd();
void DCMotorAllOff();


void DCMotorLeftRightFwdON();
void DCMotorLeftRightBkwdON();


#ifdef	__cplusplus
}
#endif

#endif	/* MOTORS_H */

