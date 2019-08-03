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
}

void UART_send_char(const unsigned char c) { //Send a single character over UART
    _tx_buffer[_tx_buffer_head] = c;
    _tx_buffer_head = (unsigned char) (_tx_buffer_head + 1) % SERIAL_TX_BUFFER_SIZE;
    TXIE = 1;

}
void UART_send_string(unsigned char *message){ //Send a full string over UART
    int i = 0;
    while(message[i]!='\0'){
        if(message[i] == '\0'){
            break;
        }
        UART_send_char(message[i]); // Call function to send a single char
        i++;
    }
}
int UART_available(void){ //Check the size of the receive buffer
    return ((unsigned int)(SERIAL_RX_BUFFER_SIZE + _rx_buffer_head - _rx_buffer_tail)) % SERIAL_RX_BUFFER_SIZE;
}
unsigned char UART_read(){ //Read a single char from UART transmit buffer
    if(_rx_buffer_head == _rx_buffer_tail){
        return -1;
    }else{
        unsigned char c = _rx_buffer[_rx_buffer_tail];
        _rx_buffer_tail = (unsigned char)(_rx_buffer_tail + 1) % SERIAL_RX_BUFFER_SIZE;
        return c;
    }
}