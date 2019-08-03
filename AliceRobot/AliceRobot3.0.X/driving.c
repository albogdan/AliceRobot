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
//        lcd_clear();
//        LCD_set_cursor(0,0);
//        printf("master: %2d", tickLeftTemp);//4
//        LCD_set_cursor(1,0);
//        printf("slave: %2d", tickRightTemp);//7
//        LCD_set_cursor(2,0);
//        printf("err: %2.2f", error/kp);
//        LCD_set_cursor(3,0);
//        printf("Dist: %4.2f", distanceL);
        
        slavePower = slavePower + (error/kp);

        //IF swerving to the LEFT --> INCREASE KP
        //IF swerving to the RIGHT --> DECREASE KP
        set_pwm_duty_cycle_RC2_RightSlave(slavePower);
        set_pwm_duty_cycle_RC1_LeftMaster(masterPower);

        tickLeftTemp = 0;
        tickRightTemp = 0;

    }
    if(travelDirection == DIRECTION_BACKWARD){
        error = tickRightTemp - tickLeftTemp;
//        lcd_clear();
//        LCD_set_cursor(0,0);
//        printf("master: %2.2f", masterPower);
//        LCD_set_cursor(1,0);
//        printf("slave: %2.2f", slavePower);
//        LCD_set_cursor(2,0);
//        printf("err: %2.2f", error/kp);
//        LCD_set_cursor(3,0);
//        printf("Dist: %4.2f", distanceL);

        slavePower = slavePower + (error/kp);

        set_pwm_duty_cycle_RC2_RightSlave(slavePower);
        set_pwm_duty_cycle_RC1_LeftMaster(masterPower);

        tickLeftTemp = 0;
        tickRightTemp = 0;

    }
    
    

    
}

 void set_pwm_duty_cycle_RC2_RightSlave(float duty){ //RIGHT MOTOR SLAVE
    if((duty >= 0) && (duty <= 100.0)){
        // Our pulse width cannot exceed the period of the wave. First we
        // compute this upper limit for the duty cycle registers (max_duty_val),
        // then we compute a percentage of this (duty_val) as per the argument
        // passed in. See datasheet pg 150-151 (equation 16-2, and figure 16-2)
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
        // Our pulse width cannot exceed the period of the wave. First we
        // compute this upper limit for the duty cycle registers (max_duty_val),
        // then we compute a percentage of this (duty_val) as per the argument
        // passed in. See datasheet pg 150-151 (equation 16-2, and figure 16-2)
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