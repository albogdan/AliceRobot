/* 
 * File:   main.c
 * Author: AB
 *
 * Created on February 4, 2019, 7:54 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include "configBits.h"
#include "lcd.h"
#include "I2C.h"
#include "stdutils.h"
#include "rtc.h"
#include <string.h>


struct Screen{
    char *name;
    char *(lineText[16]);
    int cursorLine;
    int minCursor;
    int maxCursor;
    void (*moveCursorUp)(struct Screen*);
    void (*moveCursorDown)(struct Screen*);
    void (*enterKey)(struct Screen*);
    void (*backKey) (struct Screen*);
};
typedef struct Screen Screen;
void printDateTime(Screen *currentScreen);

void printScreen(Screen *currentScreen);
void moveCursorUp(Screen *currentScreen);
void moveCursorDown(Screen *currentScreen);
void enterKey(Screen *currentScreen);
void backKey(Screen *currentScreen);

const char keys[] = "123A456B789C*0#D";
Screen homeScreen = {"HOME", 
                    {"TIME", "DATE", "Start Run", "View Logs"}, 2, 2, 3,
                    moveCursorUp,moveCursorDown, enterKey, backKey};
Screen logList = {"LOGLIST",
                 {"Log 1", "Log 2", "Log 3", "Log 4"}, 0, 0, 3,
                 moveCursorUp,moveCursorDown, enterKey, backKey};
Screen runningOperation = {"RUNNING",
                          {"Running...", "START_TIME", "CURRENT_TIME", ""}};

volatile bool key_was_pressed = false;
rtc_t rtc;

void moveCursorUp(Screen *currentScreen){
    if(currentScreen->cursorLine != currentScreen->minCursor){
        LCD_set_cursor(currentScreen->cursorLine,15);
        putch(' ');
        currentScreen->cursorLine-=1;
        LCD_set_cursor(currentScreen->cursorLine,15);
        putch('*');
    }
}
void moveCursorDown(Screen *currentScreen){
    if(currentScreen->cursorLine != currentScreen->maxCursor){
        LCD_set_cursor(currentScreen->cursorLine,15);
        putch(' ');
        currentScreen->cursorLine+=1;
        LCD_set_cursor((*currentScreen).cursorLine,15);
        putch('*');
    }
}
void enterKey(Screen *currentScreen){
    if(currentScreen->name == "HOME"){
        switch(currentScreen->cursorLine){
            case 2:
                *currentScreen = runningOperation;
                LCD_write_str("ING HERE");
                unsigned char temp[20];
                sprintf(temp, "%02x:%02x:%02x", rtc.hour, rtc.min,rtc.sec); // HH:MM:SS
                currentScreen->lineText[1] = temp;
                break;
            case 3:
                *currentScreen = logList;
                break;
            default:
                break;
        }
    }
}

void backKey(Screen *currentScreen){
    if(currentScreen->name == "LOGLIST"){
        *currentScreen = homeScreen;
    }
}

void main(void){
    // RD2 is the character LCD RS
    // RD3 is the character LCD enable (E)
    // RD4-RD7 are character LCD data lines
    LATD = 0x00;
    TRISC = 0x00;
    TRISD = 0x00;
    TRISBbits.RB1 = 1;
    // Set all A/D ports to digital (pg. 222)
    ADCON1 = 0b00001111;
    // Enable RB1 (keypad data available) interrupt
    INT1IE = 1;
    
    // Initialize LCD
    initLCD();
    I2C_Master_Init(100000);

    RTC_Init();

    // Enable interrupts
    ei();

    Screen currentScreen = homeScreen;
    
    printScreen(&currentScreen);
    
    
     /*Set the time and Date only once. Once the Time and Date is set, comment these lines
         and reflash the code. Else the time will be set every time the controller is reset*/
	rtc.hour = 0x23; //  10:40:20 am
    rtc.min =  0x39;
    rtc.sec =  0x37;

    rtc.date = 0x01; //1st Jan 2016
    rtc.month = 0x01;
    rtc.year = 0x19;
    rtc.weekDay = 1; // Friday: 5th day of week considering monday as first day.
    RTC_SetDateTime(&rtc);

    while(1){
        if(key_was_pressed){
            key_was_pressed=false;
            unsigned char keypress = (PORTB & 0xF0) >> 4;
            unsigned char temp = keys[keypress];
            if(temp=='0'){
                currentScreen.moveCursorDown(&currentScreen);
            }
            if(temp=='2'){                
                currentScreen.moveCursorUp(&currentScreen);
            }
            if(temp=='B'){
                currentScreen.backKey(&currentScreen);
                lcd_clear();
            }
            if(temp=='D'){
                currentScreen.enterKey(&currentScreen);
                lcd_clear();
            }
        }
        printScreen(&currentScreen);        
    }
}


void printScreen(Screen *currentScreen){
//    lcd_clear();
    
    LCD_set_cursor(0,0);
    if(currentScreen->name == "HOME"){
        printDateTime(currentScreen);
        LCD_write_str(currentScreen->lineText[0]);
        LCD_set_cursor(1,0);
        LCD_write_str(currentScreen->lineText[1]);
        LCD_set_cursor(2,0);    
        LCD_write_str(currentScreen->lineText[2]);
        LCD_set_cursor(3,0);
        LCD_write_str(currentScreen->lineText[3]);
        LCD_set_cursor(currentScreen->cursorLine,15);
        LCD_write_str("*");        
    }else if(currentScreen->name == "RUNNING"){
        printDateTime(currentScreen);
        LCD_write_str(currentScreen->lineText[0]);
        LCD_set_cursor(1,0);
        LCD_write_str(currentScreen->lineText[1]);
        LCD_set_cursor(2,0);    
        LCD_write_str(currentScreen->lineText[2]);

    }else{
        LCD_write_str(currentScreen->lineText[0]);
        LCD_set_cursor(1,0);
        LCD_write_str(currentScreen->lineText[1]);
        LCD_set_cursor(2,0);    
        LCD_write_str(currentScreen->lineText[2]);
        LCD_set_cursor(3,0);
        LCD_write_str(currentScreen->lineText[3]);
        LCD_set_cursor(currentScreen->cursorLine,15);
        LCD_write_str("*");
    }

}
void printDateTime(Screen *currentScreen){
    char tempTime[20], tempDate[20];
    LCD_set_cursor(0,0);
    RTC_GetDateTime(&rtc);
    if(currentScreen->name == "HOME"){
        LCD_set_cursor(0,0);
        sprintf(tempTime, "%02x:%02x:%02x", rtc.hour, rtc.min,rtc.sec); // HH:MM:SS
        currentScreen->lineText[0] = tempTime;

        sprintf(tempDate, "%02x/%02x/20%02x", (rtc.date)&0x0F+0x30, (rtc.month)&0x0F+0x30,(rtc.year)&0x0F+0x30); // HH:MM:SS
        currentScreen->lineText[1] = tempDate;
    //        printf("%02c/%02c/20%02c", rtc.date, rtc.month, rtc.year); // Print date in YY/MM/DD
    }else if(currentScreen->name == "RUNNING"){
//        sprintf(tempTime, "CT:%x:%x:%x", (rtc.hour)&0x0F+0x30, (rtc.min)&0x0F+0x30,(rtc.sec)&0x0F+0x30); // HH:MM:SS
//        LCD_write_str(tempTime);
        sprintf(tempTime, "%02x:%02x:%02x", rtc.hour, rtc.min,rtc.sec); // HH:MM:SS
        currentScreen->lineText[2] = tempTime;
    } 
}
void __interrupt() high_isr(void){

    // Interrupt on change handler for RB1
    if(INT1IF && INT1IE){
        // Notice how we keep the interrupt processing very short by simply
        // setting a "flag" which the main program loop checks
        key_was_pressed = true;
        INT1IF = 0; // Clear interrupt flag bit to signify it's been handled

    }
}