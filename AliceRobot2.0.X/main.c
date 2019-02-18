///**
// * @file
// * @author Tyler Gamvrelis
// * 
// * Created on October 1, 2017, 2:28 PM
// * 
// * @defgroup BoardTest
// * @brief Tests the basic capabilities of most of the development board modules
// */
//
//#include <xc.h>
//#include "configBits.h"
//#include "I2C.h"
//#include "lcd.h"
//
//void sendUART(char *message);
//
//void main(void){
//    // Configure pins, SFRs, and on-board modules
//    // <editor-fold defaultstate="collapsed" desc="Machine Configuration">    
//    /********************************* PIN I/O ********************************/
//    /* Write outputs to LATx, read inputs from PORTx. Here, all latches (LATx)
//     * are being cleared (set low) to ensure a controlled start-up state. */  
//    LATA = 0x00;
//    LATB = 0x00; 
//    LATC = 0x00;
//    LATD = 0x00;
//    LATE = 0x00;
//
//    /* After the states of LATx are known, the data direction registers, TRISx
//     * are configured. 0 --> output; 1 --> input. Default is  1. */
//    TRISA = 0b11111111; // All inputs
//    TRISB = 0b11110010; // RB1, RB4, RB5, RB6, RB7 as inputs (for keypad)
//    TRISC = 0b10000000; /* RC3 is SCK/SCL (SPI/I2C),
//                         * RC4 is SDI/SDA (SPI/I2C),
//                         * RC5 is SDO (SPI),
//                         * RC6 and RC7 are UART TX and RX, respectively. */
//    TRISD = 0b00000001; /* RD0 is the GLCD chip select (tri-stated so that it's
//                         * pulled up by default),
//                         * RD1 is the GLCD register select,
//                         * RD2 is the character LCD RS,
//                         * RD3 is the character LCD enable (E),
//                         * RD4-RD7 are character LCD data lines. */
//    TRISE = 0b00000100; /* RE2 is the SD card chip select (tri-stated so that
//                         * it's pulled up by default).
//                         * Note that the upper 4 bits of TRISE are special 
//                         * control registers. Do not write to them unless you 
//                         * read §9.6 in the datasheet */
//
//    /************************** A/D Converter Module **************************/
//    ADCON0 = 0x00;       // Disable ADC
//    ADCON1 = 0x0F;       // Set all A/D ports to digital (pg. 222)
//    ADCON2bits.ADFM = 1; // Right justify A/D result
//
//    CVRCON = 0x00;       // Disable comparator voltage reference (pg. 239)
//    CMCONbits.CIS = 0;   // Connect C1 Vin and C2 Vin to RA0 and RA1 (pg. 233)
//    ADCON2 = 0b10110001; // Right justify A/D result, 16TAD, FOSC/8 clock
//    
//    /****************************** USART Module ******************************/
//    long baudRate = 9600;
//    SPBRG = (unsigned char)((_XTAL_FREQ / (64 * baudRate)) - 1);
//    TXSTAbits.TX9 = 0;  // Use 8-bit transmission (8 data bits, no parity bit)
//    TXSTAbits.SYNC = 0; // Asynchronous communication
//    TXSTAbits.TXEN = 1; // Enable transmitter
//    TRISCbits.TRISC6 = 0;         // TX = output
//    RCSTAbits.SPEN = 1; // Enable serial peripheral
//    //</editor-fold>
//    
////    testUART();
//    sendUART("Hello World!\n");
//}
//
//
///**
// * @brief Sends "Hello world!" via the UART bus, which can be received by a PC
// *        connected to the USB port in the USB module
// */
//void testUART(void){
//    const char msg[] = "Hello world!"; // Create a new array of characters
//    
//    // Transmit
//    for(unsigned char i = 0; i < sizeof(msg); i++){
//        while(!TXIF | !TRMT){
//            continue;
//        }
//        
//        TXREG =  msg[i];
//    }
//}
//
//
//void sendUART(char *message){
//    // Transmit
//    int i;
//    for(i = 0; message[i]!='\0'; i++){      
//        while(!TXIF | !TRMT){
//            continue;
//        }
//        TXREG =  message[i];
//    }
//}
//
//






#include <xc.h>
#include "configBits.h"
#include "lcd.h"
#include "I2C.h"
#include "stdutils.h"
#include "rtc.h"
#include "uart.h"


//unsigned char UART_read_string(void);
//char UART_read_char(void);
void portSetup(void);

void DCMotorLeftRightFwdON();
void DCMotorLeftRightBkwdON();
void DCMotorLeftRightOFF();

/* Define constants and global variables */
volatile bool key_was_pressed = false;
rtc_t rtc;

void main(void){
    /**************************** INIT Functions ******************************/
    portSetup();
    initLCD();
    I2C_Master_Init(100000);
    RTC_Init();
    UART_Init();

    LCD_set_cursor(0,0);

    /***************************** INTERRUPTS *********************************/
    /* Enable RB1 (keypad data available) interrupt */
    INT1IE = 1;
    /* Enable General Interrupts*/
    ei();
    /* Enable Priority Interrupts*/
    RCONbits.IPEN = 1;
    /* Enable peripheral interrupts*/
    INTCONbits.PEIE_GIEL = 1;
    /***************************** MAIN PROGRAM *********************************/
    bool commandReceived = false;
    int commandLength =0 ;
    unsigned char command [50];
    int index = 0;
    while(1){
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
            commandReceived = false;
            switch(command[0]){
                case 'M': //move
                    switch(command[1]){
                        case 'F': //forwards
                            switch(command[2]){
                                case 'N': //oN
                                    DCMotorLeftRightFwdON();
                                    break;
                                case 'F': //oFF
                                    DCMotorLeftRightOFF();
                                    break;
                                default:
                                    break;
                            }
                            break;
                        case 'B': //backwards
                            switch(command[2]){
                                case 'N': //oN
                                    DCMotorLeftRightBkwdON();
                                    break;
                                case 'F': //oFF
                                    DCMotorLeftRightOFF();
                                    break;
                                default:
                                    break;
                            }
                            break;
                        default:
                            break;
                    }
                    break;
                case 'O':
                default:
                    break;
            }
        }

    }

}
/*********************COMMANDS ***********************/
/* MFN - Move Forwards On A0,A2 (Turns on Left and Right DC Motors to go FWDS)
 * MFF - Move Forwards Off (Turns off Left and Right DC Motors)
 * MBN - Move Backwards On A1,A3 (Turns on Left and Right DC Motors to go BKWDS)
 * MBF - Move Backwards Off (Turns off Left and Right DC Motors)
 * 
 */


/*****************LEFT MOTOR CONTROLS***************/
/*Left Motor Fwd	RA2 / AN2
Left Motor Bkwd	RA3 / AN3*/
void DCMotorLeftRightFwdON(){
    LATAbits.LATA1 = 0; //Turn OFF Bkwd For Right Motor
    LATAbits.LATA3 = 0; //Turn OFF Bkwd For Left Motor    

    LATAbits.LATA0 = 1; //Turn ON Fwd For Right Motor
    LATAbits.LATA2 = 1; //Turn ON Fwd For Left Motor
}
void DCMotorLeftRightBkwdON(){
    LATAbits.LATA1 = 1; //Turn ON Bkwd For Right Motor
    LATAbits.LATA3 = 1; //Turn ON Bkwd For Left Motor   

    LATAbits.LATA0 = 0; //Turn OFF Fwd For Right Motor
    LATAbits.LATA2 = 0; //Turn OFF Fwd For Left Motor 
}
void DCMotorLeftRightOFF(){
    LATAbits.LATA1 = 0; //Turn OFF Bkwd For Right Motor
    LATAbits.LATA3 = 0; //Turn OFF Bkwd For Left Motor   

    LATAbits.LATA0 = 0; //Turn OFF Fwd For Right Motor
    LATAbits.LATA2 = 0; //Turn OFF Fwd For Left Motor 
}

/*
Middle Motor Fwd	RA4
Middle Motor Bkwd	RA5 / AN4
 */


void __interrupt(high_priority) high_isr(void){
//    di();
    // Interrupt on change handler for RB1
    if(INT1IF && INT1IE){
        // Notice how we keep the interrupt processing very short by simply
        // setting a "flag" which the main program loop checks
        key_was_pressed = true;
        INT1IF = 0; // Clear interrupt flag bit to signify it's been handled
    }
//    ei();
}

void __interrupt(low_priority) low_isr(void){
    di();
    // Interrupt on change handler for RB1
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
            TXIE = 0;
        }
    }
    ei();
}



/*Functions:
 *  readRX
 *  transmitTX(char msg[])
 *  writeToLCD(line, msg)
 *  sendTime 
 *  getTime()
 
 */


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
    TRISB = 0b11110010; // RB1, RB4, RB5, RB6, RB7 as inputs (for keypad)
    TRISC = 0b10000000; /* RC3 is SCK/SCL (SPI/I2C),
                         * RC4 is SDI/SDA (SPI/I2C),
                         * RC5 is SDO (SPI),
                         * RC6 and RC7 are UART TX and RX, respectively. */
    TRISD = 0b00000000; /* RD0 is the GLCD chip select (tri-stated so that it's
                         * pulled up by default),
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
        

}