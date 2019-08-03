#include <RTClib.h>
#include "eeprom.h"
#ifndef SCREEN_H // include guard
#define SCREEN_H

#define SIZE_OF_LINES 40
class Screen{
public:
String lines[SIZE_OF_LINES]; //List of line text
int beginningDisplay;       //Where in the list the first line to display is
int endOfLines;             //Amount of lines written to the list
RTC_DS1307 rtcScreen;
long lineStarts [100];
int lenLines = 0;

Screen(String L1, String L2, String L3, String L4, int beginLocation, RTC_DS1307 rtc);
Screen(String L1, int beginLocation, RTC_DS1307 rtc);
Screen(int beginLocation, RTC_DS1307 rtc);
Screen();

void moveScreenDown();
void moveScreenUp();

void loadLogs();
void viewSpecificLog(int location, Screen *specificLog);

void setLine(int lineNumber, String text);

int addLine(String newLine);
String updateLine(int lineNumber);

void sendLine(int lineNumber);
void fullSend();

String getTime();
String getDate();
};
#endif /* SCREEN_H */
