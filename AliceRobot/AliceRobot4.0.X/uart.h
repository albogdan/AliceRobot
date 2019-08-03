/* 
 * File:   uart.h
 * Author: AB
 *
 * Created on February 12, 2019, 5:46 PM
 */

#ifndef UART_H
#define	UART_H

void UART_Init(void);
void UART_send_string(unsigned char *message);
unsigned char UART_read();
int UART_available(void);
void UART_send_char(const unsigned char c);

#if !defined(SERIAL_RX_BUFFER_SIZE)
#define SERIAL_RX_BUFFER_SIZE 64
#endif
    
#if !defined(SERIAL_TX_BUFFER_SIZE)
#define SERIAL_TX_BUFFER_SIZE 64
#endif

  
volatile unsigned char _rx_buffer[SERIAL_RX_BUFFER_SIZE];
volatile unsigned char _tx_buffer[SERIAL_TX_BUFFER_SIZE];
volatile unsigned char _rx_buffer_head;
volatile unsigned char _rx_buffer_tail;
volatile unsigned char _tx_buffer_head;
volatile unsigned char _tx_buffer_tail;

#endif	/* UART_H */

