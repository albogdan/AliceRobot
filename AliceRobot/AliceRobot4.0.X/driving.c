#include <xc.h>
#include "variables.h"
#include "driving.h"
#include "motors.h"
#include "lcd.h"
#include <stdbool.h>

const float kp = 3.5;

/* MASTER LEFT WHEEL, SLAVE RIGHT WHEEL*/
void drive(){
    float error = 0.0;

    if(travelDirection == DIRECTION_FORWARD){
        error = tickLeftTemp - tickRightTemp;
        slavePower = slavePower + (error/kp);

        set_pwm_duty_cycle_RC2_RightSlave(slavePower);
        set_pwm_duty_cycle_RC1_LeftMaster(masterPower);

        tickLeftTemp = 0;
        tickRightTemp = 0;
    }
    if(travelDirection == DIRECTION_BACKWARD){
        error = tickRightTemp - tickLeftTemp;
        slavePower = slavePower + (error/kp);

        set_pwm_duty_cycle_RC2_RightSlave(slavePower);
        set_pwm_duty_cycle_RC1_LeftMaster(masterPower);

        tickLeftTemp = 0;
        tickRightTemp = 0;
    }
}

 void set_pwm_duty_cycle_RC2_RightSlave(float duty){ //RIGHT MOTOR SLAVE
    if((duty >= 0) && (duty <= 100.0)){
        unsigned short max_duty_val = PR2 + 1;
        unsigned short duty_val = (unsigned short)(
            (duty*4 / 100.0) * (float)max_duty_val
        );
        // Save the duty cycle into the registers
        CCP1X = duty_val & 2; // Set the 2 least significant bit in CCP1CON register
        CCP1Y = duty_val & 1;
        CCPR1L = duty_val >> 2; // Set rest of the duty cycle bits in CCPR1L
    }
}

void set_pwm_duty_cycle_RC1_LeftMaster(float duty){ //LEFT MOTOR MASTER
    if((duty >= 0) && (duty <= 100.0)){
        unsigned short max_duty_val = PR2 + 1;
        unsigned short duty_val = (unsigned short)(
            (duty*4 / 100.0) * (float)max_duty_val
        );
        // Save the duty cycle into the registers
        CCP2X = duty_val & 2; // Set the 2 least significant bit in CCP1CON register
        CCP2Y = duty_val & 1;
        CCPR2L = duty_val >> 2; // Set rest of the duty cycle bits in CCPR1L
    }
}