#include "Screen.h"
#include <Wire.h>
#include "RTClib.h"

using namespace N;
using namespace std;

RTC_DS1307 rtc;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
bool stringComplete = false;
String inputString = "";
Screen homeScreen("TIME", "DATE", "1 - Start Run\n", "2 - View Logs\n", 0, rtc);
Screen runningScreen("Running...\n", "", "NOW", "\n",0, rtc);
Screen emptyScreen("                \n","                \n","                \n","                \n",0,rtc);
Screen currentScreen;
DateTime startTime;
bool firstStart;
DateTime endTime;

void setup() {
  firstStart = true; //Allows run to only start once, to prevent startTime from being overwritten

  inputString.reserve(200);

  Serial.begin(19200);
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }


  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
  }
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  currentScreen = homeScreen;
  emptyScreen.fullSend();
}


void loop() {
  String serialValue;
  if(stringComplete){
    Serial.println("GOT STRING");
    if(inputString == "K1\n" && firstStart){
      firstStart = false;
      startTime = rtc.now();
      char temp[16];
      sprintf(temp, "START: %02d:%02d:%02d\n", startTime.hour(), startTime.minute(), startTime.second());//
      String text(temp);
      runningScreen.setLine(1, text);
      emptyScreen.fullSend();
      Serial.println("CHANGING TO RUN MODE");
      currentScreen = runningScreen;
    }
    inputString = "";
    stringComplete = false;
  }
currentScreen.fullSend();
delay(500);
}

void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}
