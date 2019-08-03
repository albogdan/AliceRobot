#include <xc.h>
#include "configBits.h"
#include "lcd.h"
#include "stdutils.h"
#include <stdlib.h>
#include <stdio.h>
#include "uart.h"
#include "motors.h"
#include <string.h>
#include "variables.h"
#include "driving.h"

volatile bool key_was_pressed = false; //interrupt for key on keypad being pressed
//ticks travelled by robot given by encoders on RB2 (left motor)
volatile double tickL = 0.0; 
//ticks travelled by robot given by encoders on RB0 (right motor)
volatile double tickR = 0.0; 
//temp distances for PWM correction
volatile long int tickLeftTemp = 0;
volatile long int tickRightTemp = 0;
volatile double distanceL = 0.0; //distance from tickL
volatile double distanceR = 0.0; //distnace from tickR 
volatile float masterPower = 73.0; //PWM power master
volatile float slavePower = 73.0;  //PWM power slave

volatile int travelDirection = DIRECTION_STATIONARY;
int sideTurnedOff = STOPPED_NONE;

bool moving = false;
void portSetup(void);
void PWMSetup();
/* Define constants and global variables */

//Possible keys on the keypad mapped to their number
const char keys[] = "123A456B789C*0#D"; 

void main(void){
    /**************************** INIT Functions ******************************/
    portSetup();
    PWMSetup();
    initLCD();
    UART_Init();
    LCD_set_cursor(0,0);
    UART_send_string("Init complete\n");

    /***************************** MAIN PROGRAM *********************************/
    bool commandReceived = false;
    int commandLength =0 ;
    unsigned char command [25];
    unsigned char lcdDisplay [16];
    unsigned char uart_char_to_send[25];
    char buffer[10];
    int index = 0;
    unsigned char toPrint;
    long long int k = 0;

    while(1){
        distanceL = (tickL/GEAR_RATIO)*CIRCUMFERENCE;
        distanceR = (tickR/GEAR_RATIO)*CIRCUMFERENCE;
        if(moving){// && k == 100){
            drive();
            k=0;
        }
        k+=1;

        if(UART_available()>0){ //Check is chars available in serial
            command[index] = UART_read();
            index++;  
        }
        if(index>0 && command[index-1] == '\n'){ //check is complete command received
            commandReceived = true;
            commandLength = index;
            index = 0;
        }

        if(commandReceived){ //if a command is completed, proces it
            commandReceived = false;
            switch(command[0]){
                /*Handle motor control instructions for Left, Right, Middle DC Motors*/
                case 'M': //move
                    switch(command[1]){
                        case 'F': //forwards
                            switch(command[2]){
                                case 'B': //Both (Left and Right) motors
                                    travelDirection = DIRECTION_FORWARD;
                                    moving = true;
                                    DCMotorLeftRightFwdON();
                                    break;
                                case 'L': // Left only motors
                                    DCMotorLeftFwd();
                                    break;
                                case 'R': // Right only motors
                                    DCMotorRightFwd();
                                    break;
                                case 'F': //Off motors
                                    moving = false;
                                    DCMotorAllOff();
                                    break;
                                default:
                                    break;
                            }
                            break;
                        case 'B': //backwards
                            switch(command[2]){
                                case 'B': //Both (Left and Right)
                                    travelDirection = DIRECTION_BACKWARD;
                                    moving = true;
                                    DCMotorLeftRightBkwdON();
                                    break;
                                case 'L': // Left only motors
                                    DCMotorLeftBkwd();
                                    break;
                                case 'R': // Right only motors
                                    DCMotorRightBkwd();
                                    break;
                                case 'F': //Off motors
                                    moving = false;
                                    DCMotorAllOff();
                                    break;
                                default:
                                    break;
                            }
                            break;
                        case 'S': //to the side
                            switch(command[2]){
                                case 'L': //to the left side
                                    DCMotorCenterLeft();//Turn on center motor left
                                    break;
                                case 'R': //to the right side
                                    DCMotorCenterRight();//Turn on center motor right
                                    break;
                                case 'F': //oFF
                                    DCMotorAllOff(); //Turn off center motor
                                    break;
                                default:
                                    break;
                            }
                            break;
                        default:
                            break;
                    }
                    break;
                    
                /*Handle display instructions from UART and send text to LCD*/
                case 'D':
                    switch(command[1]){
                        case '1': //print on line 1 of LCD
                            LCD_set_cursor(0,0);
                            sprintf(lcdDisplay,"%.*s", 16, command + 2);
                            LCD_write_str(lcdDisplay);
                            break;
                        case '2': //print on line 2 of LCD
                            LCD_set_cursor(1,0);
                            sprintf(lcdDisplay,"%.*s", 16, command + 2);
                            LCD_write_str(lcdDisplay);
                            break;
                        case '3': //print on line 3 of LCD
                            LCD_set_cursor(2,0);
                            sprintf(lcdDisplay,"%.*s", 16, command + 2);
                            LCD_write_str(lcdDisplay);
                            break;
                        case '4': //print on line 4 of LCD
                            LCD_set_cursor(3,0);
                            sprintf(lcdDisplay,"%.*s", 16, command + 2);
                            LCD_write_str(lcdDisplay);
                            break;
                        default:
                            break;
                    }
                    break;
                /* Handle total distance measurement to send to ESP*/
                case 'Q': 
                    sprintf(buffer,"%c%-4.0f\n",'L', distanceL);
                    UART_send_string(buffer);
                    break;
                default:
                    break;
            }
            
        }
        /* Handle Keypresses on the keypad and send to ESP thru UART*/
        if(key_was_pressed){
            key_was_pressed = false; //Clear the flag
            unsigned char keypress = (PORTB & 0xF0) >> 4; //Get the key pressed
            sprintf(uart_char_to_send, "%c%c\n", 'K', keys[keypress]);
            UART_send_string(uart_char_to_send);
        }
    }
}
/*********************COMMANDS ***********************(INCLUDE IN FINAL REPORT)/
/* MFB - Motors Forwards Both (Left and Right Motors turn on backwards)
 * MFL - Motors Forwards Left (Left Motor turn on)
 * MFR - Motors Forwards Right (Right Motor turn on)
 * MFF - Move Forwards Off (Turns off Left and Right DC Motors)
 * 
 * MBB - Motors Backwards Both (Left and Right Motors turn on backwards)
 * MBL - Motors Backwards Left (Left Motor turn on)
 * MBR - Motors Backwards Right (Right Motor turn on)
 * MBF - Move Backwards Off (Turns off Left and Right DC Motors)
 * 
 * MSL - Motors Sideways Left (move to the left)
 * MSR - Motors Sideways Right (move to the right)
 * MSF - Motors Sideways Off
 * 
 * D1 - Print from UART on Line 1 of LCD
 * D2 - Print from UART on Line 2 of LCD
 * D3 - Print from UART on Line 3 of LCD
 * D4 - Print from UART on Line 4 of LCD
 * 
 * Q - Return distance from encoders
 */




void __interrupt() high_isr(void){
    // Interrupt on change handler for RB0
    if (INT0IE && INT0IF){
        if(PORTDbits.RD0){ //moving forward, increment ticks
            tickL++;
            tickLeftTemp++;
        }else{ //moving backwards, decrement ticks
            tickL--;
            tickLeftTemp--;
        }
        INT0IF = 0; //Clears interrupt flag
    }
    // Interrupt on change handler for RB2
    if (INT2IE && INT2IF){
        if(PORTDbits.RD1){ //moving forward, increment ticks
            tickR++;
            tickRightTemp++;
        }else{ //moving backwards, decrement ticks
            tickR--;
            tickRightTemp--;
        }
        INT2IF = 0; //Clears interrupt flag
    }
    if(INT1IF && INT1IE){    // Interrupt on change handler for RB1 keypad
        key_was_pressed = true;
        INT1IF = 0; // Clear interrupt flag bit to signify it's been handled
    }
     if(RCIE && RCIF){      //Interrupt on change handler for UART Receive
        _rx_buffer[_rx_buffer_head] = RCREG;
        _rx_buffer_head = (unsigned char)(_rx_buffer_head + 1) % SERIAL_RX_BUFFER_SIZE;
        RCIF = 0; // Clear interrupt flag bit to signify it's been handled
    }
    if(TXIE && TXIF){       //Interrupt on change handler for UART Transmit
        if((_tx_buffer_tail != _tx_buffer_head)){
            TXREG = _tx_buffer[_tx_buffer_tail];
            _tx_buffer_tail = (unsigned char)(_tx_buffer_tail + 1) % SERIAL_TX_BUFFER_SIZE;
            TXIF = 0; //Clear interrupt flag bit to signify it's been handled
        }else{
            TXIF = 0;
            TXIE = 0;
        }
    }
}

void portSetup(){
    /********************************* PIN I/O ********************************/
    /* Write outputs to LATx, read inputs from PORTx. Here, all latches (LATx)
     * are being cleared (set low) to ensure a controlled start-up state. */
    LATA = 0x00;
    LATB = 0x00; 
    LATC = 0x00;
    LATD = 0x00;
    LATE = 0x00;
 
    /* After the states of LATx are known, the data direction registers, TRISx
     * are configured. 0 --> output; 1 --> input. Default is  1. */
    TRISA = 0b11000000; /* RA0, RA1 for the Right Motor control
                         * RA2, RA3 for the Left Motor control
                         * RA4, RA5 for the middle motor control */
    TRISB = 0b11110111; /* RB1, RB4, RB5, RB6, RB7 as inputs (for keypad)
                         * RB0, RB2 as inputs (for encoders)*/
    TRISC = 0b10000000; /* RC3 is SCK/SCL (SPI/I2C),
                         * RC4 is SDI/SDA (SPI/I2C),
                         * RC5 is SDO (SPI),
                         * RC6 and RC7 are UART TX and RX, respectively. */
    TRISD = 0b00000011; /* RD0, RD1 as inputs (for encoders)
                         * RD1 is the GLCD register select,
                         * RD2 is the character LCD RS,
                         * RD3 is the character LCD enable (E),
                         * RD4-RD7 are character LCD data lines. */
    /************************** A/D Converter Module **************************/
    ADCON0 = 0x00;       // Disable ADC
    ADCON1 = 0x0F;       // Set all A/D ports to digital (pg. 222)
    ADCON2bits.ADFM = 1; // Right justify A/D result

    CVRCON = 0x00;       // Disable comparator voltage reference (pg. 239)
    CMCONbits.CIS = 0;   // Connect C1 Vin and C2 Vin to RA0 and RA1 (pg. 233)
    ADCON2 = 0b10110001; // Right justify A/D result, 16TAD, FOSC/8 clock

     /***************************** INTERRUPTS *********************************/
    /*Setup interrupts for the encoders at RB0 and RB2*/
    INT0IF = 0; //Clears interrupt 0 flag bit (Must occur before enabling interrupt)
    INT0IE = 1; //Enables the INT0 external interrupt
    INT2IF = 0; //Clears interrupt 2 flag bit (Must occur before enabling interrupt)
    INT2IE = 1; //Enables the INT2 external interrupt
    
    INTEDG0 = 1; //Sets external interrupt 0 to interrupt on rising edge
    
    /* Enable RB1 (keypad data available) interrupt */
    INT1IF = 0;
    INT1IE = 1;
    
    INTEDG1 = 1;
    /* Enable General Interrupts*/
    ei();
    /* Enable peripheral interrupts*/
    INTCONbits.PEIE_GIEL = 1;
}
 void PWMSetup(){
    // Disable output from PWM pin while we are setting up PWM
    TRISCbits.TRISC2 = 1;
    TRISCbits.TRISC1 = 1;
    
    // Configure PWM frequency, 3.1 kHz. See datasheet pg. 149, equation 16-1
    const unsigned long FREQUENCY = 3100;
    const unsigned char TIMER2_PRESCALER = 16;
    PR2 = (_XTAL_FREQ / (FREQUENCY * 4 * TIMER2_PRESCALER)) - 1;
    
    // Configure CCP1CON, single output mode, all active high
    P1M1 = 0;
    P1M0 = 0;
    CCP1M3 = 1;
    CCP1M2 = 1;
    CCP1M1 = 0;
    CCP1M0 = 0;

    // Configure CCP2CON, single output mode, all active high
    CCP2M3 = 1;
    CCP2M2 = 1;
    CCP2M1 = 0;
    CCP2M0 = 0;

    // Set timer 2 prescaler to 16
    T2CKPS0 = 1;
    T2CKPS1 = 1;

    // Enable timer 2
    TMR2ON = 1;

    // Enable PWM output pin since setup is done
    TRISCbits.TRISC2 = 0;
    TRISCbits.TRISC1 = 0;
 }
 
