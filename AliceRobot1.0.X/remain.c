


///* 
// * File:   main.c
// * Author: AB
// *
// * Created on January 30, 2019, 11:40 PM
// */
//
//#include <stdio.h>
//#include <stdlib.h>
//#include <xc.h>
//#include "configBits.h"
//#include "lcd.h"
//#include "I2C.h"
//
//struct Screen{
//    char *name;
//    char *L1;
//    char *L2;
//    char *L3;
//    char *L4;
//    int cursorRow;
//    int cursorColumn;
//};
//typedef struct Screen Screen;
//
//
//const char keys[] = "123A456B789C*0#D"; 
//const char *monthNames[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
//
//
//void printScreen(Screen *currentScreen);
//void updateTime(Screen *currentScreen);
//void initHomeScreen(Screen *currentScreen, unsigned char *L1, unsigned char *L2, unsigned char *L3, unsigned char *L4);
//
//void main(void){
//    // RD2 is the character LCD RS
//    // RD3 is the character LCD enable (E)
//    // RD4-RD7 are character LCD data lines
//    LATD = 0x00;
//    TRISD = 0x00;
//    
//    // Set all A/D ports to digital (pg. 222)
//    ADCON1 = 0b00001111;
//    
//    // Initialize LCD
//    initLCD();
//    I2C_Master_Init(100000);
//
//    Screen homeScreen = {"HOME", "TIME", "DATE", "Start Run", "View Logs", 1,15};
//
//    
////    unsigned char *L1= "Date goes here";
////    unsigned char *L2= "Start Run";
////    unsigned char *L3= "View Logs";
////    unsigned char *L4= "";
//
// //   initHomeScreen(&homeScreen, L1, L2,L3,L4);
//    printScreen(&homeScreen);
//    // Main loop
//    while(1){
//        // RB1 is the interrupt pin, so if there is no key pressed, RB1 will be
//        // 0. The PIC will wait and do nothing until a key press is signaled
//        while(PORTBbits.RB1 == 0){  continue;   }
//        
//        // Read the 4-bit character code
//        unsigned char keypress = (PORTB & 0xF0) >> 4;
//        
//        // Wait until the key has been released
//        while(PORTBbits.RB1 == 1){  continue;   }
//        
//        Nop(); // Apply breakpoint here to prevent compiler optimizations
//        
//        unsigned char temp = keys[keypress];
//        putch(temp); // Push the character to be displayed on the LCD
//    }
//}
//
////void initHomeScreen(Screen *currentScreen, unsigned char *L1, unsigned char *L2, unsigned char *L3, unsigned char *L4){
////    currentScreen->L1="Date goes here";
////    currentScreen->L2=L2;
////    currentScreen->L3=L3;
////    currentScreen->cursorColumn = 15;
////    currentScreen->cursorRow = 1;
////}
//void printScreen(Screen *currentScreen){
//    if(currentScreen->name == "HOME"){
//        updateTime(currentScreen);
//        LCD_set_cursor(0,14);
//        LCD_write_str("WOK");
//
//    }
//    LCD_set_cursor(0,0);
//    LCD_write_str(currentScreen->L1);
//    LCD_set_cursor(1,0);
//    LCD_write_str(currentScreen->L2);
//    LCD_set_cursor(2,0);
//    LCD_write_str(currentScreen->L3);
//    LCD_set_cursor(3,0);
//    LCD_write_str(currentScreen->L4);
//    LCD_set_cursor(currentScreen->cursorRow, currentScreen->cursorColumn);
//    putch('*');
//}
//
//void updateTime(Screen *currentScreen) {
//    unsigned char time[7]; // Create a byte array to hold time read from RTC
//    // Reset RTC memory pointer
//    I2C_Master_Start(); // Start condition
//    I2C_Master_Write(0b11010000); // 7 bit RTC address + Write
//    I2C_Master_Write(0x00); // Set memory pointer to seconds
//    I2C_Master_Stop(); // Stop condition
//
//    // Read current time
//    I2C_Master_Start(); // Start condition
//    I2C_Master_Write(0b11010001); // 7 bit RTC address + Read
//    for (unsigned char i = 0; i < 6; i++) {
//        time[i] = I2C_Master_Read(ACK); // Read with ACK to continue reading
//    }
//    time[6] = I2C_Master_Read(NACK); // Final Read with NACK
//    I2C_Master_Stop(); // Stop condition
//
//    
//    sprintf(currentScreen->L1,"%02x:%02x:%02x", time[2], time[1], time[0]); // HH:MM:SS
//    sprintf(currentScreen->L2,"%03s %02x, 20%02x", monthNames[time[5] - 1], time[4], time[6]); // Print date in YY/MM/DD
//    LCD_write_str(currentScreen->L1);
//    while(1);
//}