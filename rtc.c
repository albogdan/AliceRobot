/* 
 * File:   rtc.c
 * Author: AB
 *
 * Created on December 27, 2018, 4:48 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include "rtc.h"
#include "I2C.h"
#include "stdutils.h"



/***************************************************************************************************
                             Revision History
****************************************************************************************************
15.0: Initial version 
***************************************************************************************************/


/***************************************************************************************************
                         void RTC_Init(void)
****************************************************************************************************
 * I/P Arguments: none.
 * Return value    : none

 * description :This function is used to Initialize the Ds1307 RTC.
***************************************************************************************************/
void RTC_Init(void)
{
    I2C_Master_Start();                            // Start I2C communication

    I2C_Master_Write(C_Ds1307WriteMode_U8);        // Connect to DS1307 by sending its ID on I2c Bus
    I2C_Master_Write(C_Ds1307ControlRegAddress_U8);// Select the Ds1307 ControlRegister to configure Ds1307

    I2C_Master_Write(0x00);                        // Write 0x00 to Control register to disable SQW-Out

    I2C_Master_Stop();                             // Stop I2C communication after initializing DS1307
}














/***************************************************************************************************
                    void RTC_SetDateTime(rtc_t *rtc)
****************************************************************************************************
 * I/P Arguments: rtc_t *: Pointer to structure of type rtc_t. Structure contains hour,min,sec,day,date,month and year 
 * Return value    : none

 * description  :This function is used to update the Date and time of Ds1307 RTC.
                 The new Date and time will be updated into the non volatile memory of Ds1307.
        Note: The date and time should be of BCD format, 
             like 0x12,0x39,0x26 for 12hr,39min and 26sec.    
                  0x15,0x08,0x47 for 15th day,8th month and 47th year.                 
***************************************************************************************************/
void RTC_SetDateTime(rtc_t *rtc)
{
    I2C_Master_Start();                          // Start I2C communication

    I2C_Master_Write(C_Ds1307WriteMode_U8);      // connect to DS1307 by sending its ID on I2c Bus
    I2C_Master_Write(C_Ds1307SecondRegAddress_U8); // Request sec RAM address at 00H

    I2C_Master_Write(rtc->sec);                    // Write sec from RAM address 00H
    I2C_Master_Write(rtc->min);                    // Write min from RAM address 01H
    I2C_Master_Write(rtc->hour);                    // Write hour from RAM address 02H
    I2C_Master_Write(rtc->weekDay);                // Write weekDay on RAM address 03H
    I2C_Master_Write(rtc->date);                    // Write date on RAM address 04H
    I2C_Master_Write(rtc->month);                    // Write month on RAM address 05H
    I2C_Master_Write(rtc->year);                    // Write year on RAM address 06h

    I2C_Master_Stop();                              // Stop I2C communication after Setting the Date
}







/***************************************************************************************************
                     void RTC_GetDateTime(rtc_t *rtc)
****************************************************************************************************
 * I/P Arguments: rtc_t *: Pointer to structure of type rtc_t. Structure contains hour,min,sec,day,date,month and year 
 * Return value    : none

 * description  :This function is used to get the Date(d,m,y) from Ds1307 RTC.

    Note: The date and time read from Ds1307 will be of BCD format, 
          like 0x12,0x39,0x26 for 12hr,39min and 26sec.    
               0x15,0x08,0x47 for 15th day,8th month and 47th year.              
***************************************************************************************************/
void RTC_GetDateTime(rtc_t *rtc)
{
    I2C_Master_Start();                            // Start I2C communication

    I2C_Master_Write(C_Ds1307WriteMode_U8);        // connect to DS1307 by sending its ID on I2c Bus
    I2C_Master_Write(C_Ds1307SecondRegAddress_U8); // Request Sec RAM address at 00H

    I2C_Master_Stop();                                // Stop I2C communication after selecting Sec Register

    I2C_Master_Start();                            // Start I2C communication
    I2C_Master_Write(C_Ds1307ReadMode_U8);            // connect to DS1307(Read mode) by sending its ID

    rtc->sec = I2C_Master_Read(ACK);                // read second and return Positive ACK
    rtc->min = I2C_Master_Read(ACK);                 // read minute and return Positive ACK
    rtc->hour= I2C_Master_Read(ACK);               // read hour and return Negative/No ACK
    rtc->weekDay = I2C_Master_Read(ACK);           // read weekDay and return Positive ACK
    rtc->date= I2C_Master_Read(ACK);              // read Date and return Positive ACK
    rtc->month=I2C_Master_Read(ACK);            // read Month and return Positive ACK
    rtc->year =I2C_Master_Read(NACK);             // read Year and return Negative/No ACK

    I2C_Master_Stop();                              // Stop I2C communication after reading the Date
}
