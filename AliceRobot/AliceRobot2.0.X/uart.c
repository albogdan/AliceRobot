/* 
 * File:   uart.c
 * Author: AB
 *
 * Created on February 12, 2019, 5:46 PM
 */
#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include "uart.h"
#include "configBits.h"
#include "lcd.h"


void UART_Init(void){
    _rx_buffer_head = 0;
    _rx_buffer_tail = 0;
    _tx_buffer_head = 0;
    _tx_buffer_tail = 0;
    long baudRate = 19200;
//    unsigned char x;
//    x = (unsigned char)((_XTAL_FREQ / (64 * baudRate)) - 1);   //SPBRG for Low Baud Rate
//    if(x>255){                                       //If High Baud Rate Required
//        x = (unsigned char)((_XTAL_FREQ / (16 * baudRate)) - 1); //SPBRG for High Baud Rate
//        BRGH = 1;                                     //Setting High Baud Rate IN TXSTA
//    }
//    SPBRG = x;
    SPBRG = (unsigned char)((_XTAL_FREQ / (64 * baudRate)) - 1);
    
    //SETUP TX AND RX DIRECTION
    TRISCbits.TRISC6 = 0;         // TX = output
    TRISCbits.TRISC7 = 1;         // RX = input    

    //TXSTA REGISTER SETUP
    TXSTAbits.TX9 = 0;  // Use 8-bit transmission (8 data bits, no parity bit)
    TXSTAbits.SYNC = 0; // Asynchronous communication

    TXSTAbits.TXEN = 1; // Enable transmitter
    __delay_ms(5);
    //RCSTA REGISTER SETUP
    RCSTAbits.SPEN = 1; // Enable serial peripheral port
    RCSTAbits.RX9 = 0; // Use 8-bit reception
    RCSTAbits.CREN = 1; //Use continuous reception
    
    PIE1bits.RCIE = 1; //Enable Receive Interrupts
    PIE1bits.TXIE = 1; //Enable Transmit Interrupts
    
    IPR1bits.TXIP = 0; //Set low transmit interrupt priority
    IPR1bits.RCIP = 0; //Set low receive interrupt priority
    
}


/**
 * Send a character
 * @param c character to send
 */
void UART_send_char(const unsigned char c) {
    _tx_buffer[_tx_buffer_head] = c;
    _tx_buffer_head = (unsigned char) (_tx_buffer_head + 1) % SERIAL_TX_BUFFER_SIZE;
    TXIE = 1;

}

void UART_send_string(unsigned char *message){
    // Transmit
    int i;
    i=0;
    while(message[i]!='\0'){// || message[i]!='\n'){
        if(message[i] == '\0'){// || message[i]=='\n'){
            break;
        }
 //       LCD_set_cursor(4,0);
 //       LCD_write_str("sending");
        UART_send_char(message[i]);
        i++;
    }
//    for(i = 0; message[i]!='\0'; i++){      
//        UART_send_char(message[i]);
//        i++;
//    }
    
}

int UART_available(void){
    return ((unsigned int)(SERIAL_RX_BUFFER_SIZE + _rx_buffer_head - _rx_buffer_tail)) % SERIAL_RX_BUFFER_SIZE;
}

unsigned char UART_read(){
    if(_rx_buffer_head == _rx_buffer_tail){
        return -1;
    }else{
        unsigned char c = _rx_buffer[_rx_buffer_tail];
        _rx_buffer_tail = (unsigned char)(_rx_buffer_tail + 1) % SERIAL_RX_BUFFER_SIZE;
        return c;
    }
}

//void UART_getLine(unsigned char *destination){
//    int index = 0;
//    while(1){
//        if(UART_available() >0){
//            destination[index] = UART_read();
//            index++;        
//        }
//        if(destination[index-1] == '\n'){
//            break;
//        }
//    }
//    
//}




/*Serial functions:
 * Serial.begin();          DONE
 * Serial.available();      DONE
 * Serial.read();           DONE
 * Serial.write();          DONE
 * 
 */


/**
* Determines whether a new char is available to read, and reads that char
* @return byte read
*/
//char UART_read_char(void){
//    while(PIR1bits.RCIF == 0){    // wait for a char
//        if (RCSTAbits.OERR == 1) {
//            RCSTAbits.OERR = 0; // clear overrun if it occurs
//            RCSTAbits.CREN = 0;
//            RCSTAbits.CREN = 1;
//        }
//    }
//    return RCREG;
//}


/**
 * Read a string, until we read '\0', up to max_length characters. 
 * Null termination will use 1 byte, max_length must be set accordingly.
 * @param buf   pre-allocated buffer
 * @param max_length
 * @return number of bytes read
 */
//unsigned char UART_read_string(char *buffer, int maxLength){
//    int i = 0;
//    char temp = 1;
//    for(i=0; i<maxLength; i++){
//        temp = UART_read_char();
//        if (temp == '\0' || temp == '\n' || temp == '\r') {
//            break;
//        }
//        buffer[i] = temp;
//    }
//    buffer[i+1] = '\0';
//    
//    return i;
//}
