/* 
 * File:   driving.h
 * Author: AB
 *
 * Created on March 15, 2019, 4:33 PM
 */

#ifndef DRIVING_H
#define	DRIVING_H

#ifdef	__cplusplus
extern "C" {
#endif

    void drive();
    bool correctingDirection = false;
    void set_pwm_duty_cycle_RC2_RightSlave(float duty); //RIGHT MOTOR
    void set_pwm_duty_cycle_RC1_LeftMaster(float duty); //LEFT MOTOR



#ifdef	__cplusplus
}
#endif

#endif	/* DRIVING_H */

