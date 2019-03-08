#include <RTClib.h>

#include "Screen.h"
#include <Wire.h>
#include <FS.h>   // Include the SPIFFS library
//using namespace N;
//using namespace std;

#define MODE_HOME 1
#define MODE_RUNNING 2
#define MODE_LOGS 3
#define MODE_SPECIFIC_LOG 4

RTC_DS1307 rtc;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
bool stringComplete = false;
String inputString = "";
Screen homeScreen("TIME", "DATE", "1 - Start Run\n", "2 - View Logs\n", 0, rtc);
Screen runningScreen("Running...\n", "", "NOW", "\n",0, rtc);
Screen emptyScreen("                \n","                \n","                \n","                \n",0,rtc);
Screen logScreen;
Screen specificLog;
Screen currentScreen;
int DeviceMode;
DateTime startTime;
bool firstStart;
DateTime endTime;
String characters = "0123456789";

void setup() {
    DeviceMode = 1;
    firstStart = true; //Allows run to only start once, to prevent startTime from being overwritten

    inputString.reserve(200);

    Serial.begin(19200);
    delay(10);


    if (! rtc.begin()) {
      Serial.println("Couldn't find RTC");
      while (1);
    }
    if (! rtc.isrunning()) {
      Serial.println("RTC is NOT running!");
    }
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    Serial.println("RTC Started");


    SPIFFS.begin(); //begin SPI Flash Files System
    delay(10);
    Serial.println("SPIFFS started");
      
    currentScreen = homeScreen;
    emptyScreen.fullSend();

}

void loop() {
  String serialValue;
  if(stringComplete){
    if(DeviceMode == MODE_HOME){
        if(inputString == "K1\n" && firstStart){
            DeviceMode = MODE_RUNNING;
            firstStart = false;
            startTime = rtc.now();
            char temp[16];
            sprintf(temp, "START: %02d:%02d:%02d\n", startTime.hour(), startTime.minute(), startTime.second());//
            String text(temp);
            runningScreen.setLine(1, text);
            emptyScreen.fullSend();
            Serial.println("CHANGING TO RUN MODE");
            currentScreen = runningScreen;
        }else if(inputString == "K2\n"){
            DeviceMode = MODE_LOGS;
            logScreen.loadLogs();
            currentScreen = logScreen;
        }
    }else if(DeviceMode == MODE_LOGS){
        if(inputString == "KD\n"){ //If pressed down button
            currentScreen.moveScreenDown();
        }else if(inputString == "KA\n"){ //If pressed up button
            currentScreen.moveScreenUp();
        }else if(inputString == "KB\n"){
            DeviceMode = MODE_HOME;
            currentScreen = homeScreen;
            emptyScreen.fullSend();
        }else if(inputString.substring(0,1) == "K"){
            int keyValue;
            Serial.println("Keyvalue");
            keyValue = characters.indexOf(inputString.substring(1,2))-1;
            if(keyValue != -1){
                specificLog = Screen();
                Serial.println("entering specific log fcn");
                currentScreen.viewSpecificLog(keyValue, &specificLog);
            }
            DeviceMode = MODE_SPECIFIC_LOG;
            currentScreen = specificLog;
            emptyScreen.fullSend();
        }
    }else if(DeviceMode == MODE_SPECIFIC_LOG){
        if(inputString == "KD\n"){ //If pressed down button
            currentScreen.moveScreenDown();
            emptyScreen.fullSend();
        }else if(inputString == "KA\n"){ //If pressed up button
            currentScreen.moveScreenUp();
            emptyScreen.fullSend();
        }else if(inputString == "KB\n"){
            DeviceMode = MODE_LOGS;
            currentScreen = logScreen;
        }
    }
    inputString = "";
    stringComplete = false;
  }
  currentScreen.fullSend();
//  delay(500);




  if(Serial.available()){
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
  yield();
}
