#include <xc.h>
#include "configBits.h"
#include "lcd.h"
#include "I2C.h"
#include "stdutils.h"
#include <stdlib.h>
#include <stdio.h>
#include "rtc.h"
#include "uart.h"
#include "motors.h"
#include <string.h>


//unsigned char UART_read_string(void);
//char UART_read_char(void);
#define M_PI 3.14159265358979323846
#define GEAR_RATIO 960//960 b/c 8 pulses per encoder rotation * 120 gearing ratio//120
#define CIRCUMFERENCE 200//63*M_PI
#define ACTIVATION_TOLERANCE 8.0
#define SHUTDOWN_TOLERANCE 5.0


void portSetup(void);
void reverse(char s[]);
void itoa(int n, char s[]);
double absolute(double value);
/* Define constants and global variables */
volatile bool key_was_pressed = false; //interrupt for key on keypad being pressed
const char keys[] = "123A456B789C*0#D"; //Possible keys on the keypad mapped to their number


volatile double tickL = 0.0; //distance travelled by robot given by encoders on RB2 (left motor)
volatile double tickR = 0.0; //distance travelled by robot given by encoders on RB0 (right motor)

volatile double distanceL = 0.0;
volatile double distanceR = 0.0;

volatile int travelDirection = DIRECTION_STATIONARY;
int sideTurnedOff = STOPPED_NONE;

//(DISTANCE/GEAR_RATIO)*CIRCUMFERENCE
// GEAR_RATIO = 120;
// CIRCUMFERENCE = 63*pi = 197.92


//volatile bool direction = true; //direction of travel --> True = Fwd; False = Bkwd
//rtc_t rtc;

void main(void){
    /**************************** INIT Functions ******************************/
    portSetup();
    initLCD();
    I2C_Master_Init(100000);
    RTC_Init();
    UART_Init();

    LCD_set_cursor(0,0);

//    unsigned char print [25];
//    sprintf(print, "Init Complete\n%c", '\n');
    UART_send_string("Init complete\n");

    __delay_ms(500);
    /***************************** MAIN PROGRAM *********************************/
    bool commandReceived = false;
    int commandLength =0 ;
    unsigned char command [25];
    unsigned char lcdDisplay [16];
    unsigned char uart_char_to_send[25];
    char buffer[10];
    int index = 0;
    unsigned char toPrint;
    bool correctingDirection = false;
    bool moving = false;
    while(1){
        distanceL = (tickL/GEAR_RATIO)*CIRCUMFERENCE;
        distanceR = (tickR/GEAR_RATIO)*CIRCUMFERENCE;
        if(moving && travelDirection == DIRECTION_FORWARD){
            if(distanceR - distanceL > ACTIVATION_TOLERANCE){
                DCMotorRightOFF();
                sideTurnedOff = STOPPED_RIGHT;
                correctingDirection = true;
            }
            if(distanceL - distanceR > ACTIVATION_TOLERANCE){
                DCMotorLeftOFF();
                sideTurnedOff = STOPPED_LEFT;
                correctingDirection = true;
            }
            if(correctingDirection && sideTurnedOff == STOPPED_LEFT && (distanceL - distanceR < SHUTDOWN_TOLERANCE)){
                DCMotorLeftRightFwdON();
                sideTurnedOff = STOPPED_NONE;
                correctingDirection = false;
            }
            if(correctingDirection && sideTurnedOff == STOPPED_RIGHT && (distanceR - distanceL < SHUTDOWN_TOLERANCE)){
                DCMotorLeftRightFwdON();
                sideTurnedOff = STOPPED_NONE;
                correctingDirection = false;
            }
        }
        if(moving && travelDirection == DIRECTION_BACKWARD){
            if(distanceR - distanceL > ACTIVATION_TOLERANCE){
                DCMotorLeftOFF();
                sideTurnedOff = STOPPED_LEFT;
                correctingDirection = true;
            }
            if(distanceL - distanceR> ACTIVATION_TOLERANCE){
                DCMotorRightOFF();
                sideTurnedOff = STOPPED_RIGHT;
                correctingDirection = true;
            }
            if(correctingDirection && sideTurnedOff == STOPPED_LEFT && (distanceL - distanceR < SHUTDOWN_TOLERANCE)){
                DCMotorLeftRightBkwdON();
                sideTurnedOff = STOPPED_NONE;
                correctingDirection = false;
            }
            if(correctingDirection && sideTurnedOff == STOPPED_RIGHT && (distanceR - distanceL < SHUTDOWN_TOLERANCE)){
                DCMotorLeftRightBkwdON();
                sideTurnedOff = STOPPED_NONE;
                correctingDirection = false;
            }
            
        }
        
        
        
        
 //       snprintf(buffer, 5, "%000.00f", (distanceL/GEAR_RATIO)*CIRCUMFERENCE);
//        distanceL = (tickL/GEAR_RATIO)*CIRCUMFERENCE;
//        itoa(distanceL,buffer);   // 10 means decimal
//        printf("Binary value = %s\n", buffer);
//        toPrint = distanceL + '0';


//        LCD_set_cursor(0,9);
//        printf("L%4.f", (tickL/GEAR_RATIO)*CIRCUMFERENCE);
//        LCD_set_cursor(1,9);
//        printf("R%4.f", (tickR/GEAR_RATIO)*CIRCUMFERENCE);
  
        
        if(UART_available()){
            command[index] = UART_read();
            index++;        
        }
        if(index>0 && command[index-1] == '\n'){
            commandReceived = true;
            commandLength = index;
            index = 0;
        }

        if(commandReceived){
//            UART_send_string(command);
            commandReceived = false;
            switch(command[0]){
                /*Handle motor control instructions for Left, Right, Middle DC Motors*/
                case 'M': //move
                    switch(command[1]){
                        case 'F': //forwards
                            switch(command[2]){
                                case 'B': //Both (Left and Right)
                                    LCD_set_cursor(0,0);
                                    //LCD_write_str("Move L+R Fwd   ");
                                    travelDirection = DIRECTION_FORWARD;
                                    moving = true;
                                    DCMotorLeftRightFwdON();
                                    break;
                                case 'L': // Left only
                                    LCD_set_cursor(0,0);
                                    //LCD_write_str("Move Left Fwd  ");
                                    DCMotorLeftFwd();
                                    break;
                                case 'R': // Right only
                                    LCD_set_cursor(0,0);
                                    //LCD_write_str("Move Right Fwd   ");
                                    DCMotorRightFwd();
                                    break;
                                case 'F': //oFF
                                    LCD_set_cursor(0,0);
                                    //LCD_write_str("Stop Fwd move   ");
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
                                    LCD_set_cursor(0,0);
                                    //LCD_write_str("Move L+R Bkwd  ");
                                    travelDirection = DIRECTION_BACKWARD;
                                    moving = true;
                                    DCMotorLeftRightBkwdON();
                                    break;
                                case 'L': // Left only
                                    LCD_set_cursor(0,0);
                                    //LCD_write_str("Move Left Bkwd ");
                                    DCMotorLeftBkwd();
                                    break;
                                case 'R': // Right only
                                    LCD_set_cursor(0,0);
                                    //LCD_write_str("Move Right Bkwd  ");
                                    DCMotorRightBkwd();
                                    break;
                                case 'F': //oFF
                                    LCD_set_cursor(0,0);
                                    //LCD_write_str("Stop Bkwd move  ");
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
                                    LCD_set_cursor(0,0);
                                    //LCD_write_str("Mid motor left ");
                                    DCMotorCenterLeft();//*****************
                                    break;
                                case 'R': //to the right side
                                    LCD_set_cursor(0,0);
                                    //LCD_write_str("Mid motor right");
                                    DCMotorCenterRight();//*****************
                                    break;
                                case 'F': //oFF
                                    LCD_set_cursor(0,0);
                                    //LCD_write_str("Mid motor stop");
                                    DCMotorAllOff(); //*****************
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
                        case '1':
                            LCD_set_cursor(0,0);
                            sprintf(lcdDisplay,"%.*s", 16, command + 2);
                            LCD_write_str(lcdDisplay);
                            break;
                        case '2':
                            LCD_set_cursor(1,0);
                            sprintf(lcdDisplay,"%.*s", 16, command + 2);
                            LCD_write_str(lcdDisplay);
                            break;
                        case '3':
                            LCD_set_cursor(2,0);
                            sprintf(lcdDisplay,"%.*s", 16, command + 2);
                            LCD_write_str(lcdDisplay);
                            break;
                        case '4':
                            LCD_set_cursor(3,0);
                            sprintf(lcdDisplay,"%.*s", 16, command + 2);
                            LCD_write_str(lcdDisplay);
                            break;
                        default:
                            break;
                    }
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
 */




void __interrupt(high_priority) high_isr(void){
    di();
    // Interrupt on change handler for RB0
    if (INT0IE && INT0IF){
        if(PORTDbits.RD0){
            tickL++;
        }else{
            tickL--;
        }
        INT0IF = 0; //Clears interrupt flag
    }
    // Interrupt on change handler for RB2
    if (INT2IE && INT2IF){
        if(PORTDbits.RD1){
            tickR++;
        }else{
            tickR--;
        }
        INT2IF = 0; //Clears interrupt flag
    }
    // Interrupt on change handler for RB1
    if(INT1IF && INT1IE){
        // Notice how we keep the interrupt processing very short by simply
        // setting a "flag" which the main program loop checks
        key_was_pressed = true;
        INT1IF = 0; // Clear interrupt flag bit to signify it's been handled
    }
     if(RCIE && RCIF){     
        _rx_buffer[_rx_buffer_head] = RCREG;
        _rx_buffer_head = (unsigned char)(_rx_buffer_head + 1) % SERIAL_RX_BUFFER_SIZE;
        RCIF = 0; // Clear interrupt flag bit to signify it's been handled
    }
    if(TXIE && TXIF){
        if((_tx_buffer_tail != _tx_buffer_head)){
            TXREG = _tx_buffer[_tx_buffer_tail];
            _tx_buffer_tail = (unsigned char)(_tx_buffer_tail + 1) % SERIAL_TX_BUFFER_SIZE;
            TXIF = 0; //Clear interrupt flag bit to signify it's been handled
        }else{
            TXIF = 0;//*******************************************************************************************************************
            TXIE = 0;
        }
    }

    ei();
}
//
//void __interrupt(low_priority) low_isr(void){
//    di();
//    // Interrupt on change handler for RB1
//     if(RCIE && RCIF){     
//        _rx_buffer[_rx_buffer_head] = RCREG;
//        _rx_buffer_head = (unsigned char)(_rx_buffer_head + 1) % SERIAL_RX_BUFFER_SIZE;
//        RCIF = 0; // Clear interrupt flag bit to signify it's been handled
//    }
//    if(TXIE && TXIF){
//        if((_tx_buffer_tail != _tx_buffer_head)){
//            TXREG = _tx_buffer[_tx_buffer_tail];
//            _tx_buffer_tail = (unsigned char)(_tx_buffer_tail + 1) % SERIAL_TX_BUFFER_SIZE;
//            TXIF = 0; //Clear interrupt flag bit to signify it's been handled
//        }else{
//            TXIF = 0;//*******************************************************************************************************************
//            TXIE = 0;
//        }
//    }
//
//    ei();
//}




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
    /* Enable Priority Interrupts*/
//    RCONbits.IPEN = 1;
    /* Enable peripheral interrupts*/
    INTCONbits.PEIE_GIEL = 1;
    
}


 void itoa(int n, char s[])
 {
     int i, sign;

     if ((sign = n) < 0)  /* record sign */
         n = -n;          /* make n positive */
     i = 0;
     do {       /* generate digits in reverse order */
         s[i++] = n % 10 + '0';   /* get next digit */
     } while ((n /= 10) > 0);     /* delete it */
     if (sign < 0)
         s[i++] = '-';
     s[i] = '\0';
     reverse(s);
}  

 /* reverse:  reverse string s in place */
 void reverse(char s[])
 {
     int i, j;
     char c;

     for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
         c = s[i];
         s[i] = s[j];
         s[j] = c;
     }
}  
 
 
 double absolute(double value){
     if(value< 0.0){
         return -1.0 * value;
     }else{
         return value;
     }
 }