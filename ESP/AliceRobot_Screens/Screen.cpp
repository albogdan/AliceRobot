#include <WString.h>

#include "Screen.h"
using namespace N;



/* SCREEN CONSTRUCTORS
 * L1 through L4 are the line strings and beginLocation is the location 
 * you want "cursor" to start from
 */
Screen::Screen(String L1, String L2, String L3, String L4, int beginLocation, RTC_DS1307 rtc){
  lines[0] = L1;
  lines[1] = L2;
  lines[2] = L3;
  lines[3] = L4;
  beginningDisplay = beginLocation;
  endOfLines = 4;
  rtcScreen = rtc;
}


 
Screen::Screen(String L1, int beginLocation, RTC_DS1307 rtc){
  lines[0] = L1;
  lines[1] = "";
  lines[2] = "";
  lines[3] = "";
  beginningDisplay = beginLocation;
  endOfLines = 1;
  rtcScreen = rtc;
}
Screen::Screen(int beginLocation, RTC_DS1307 rtc){
  beginningDisplay = beginLocation;
  endOfLines = 0;
  rtcScreen = rtc;
}
Screen::Screen(){}






/* ADD LINE TO THE LIST OF LINES
 * newLine is the string you wish to add
 */
int Screen::addLine(String newLine){
  if(SIZE_OF_LINES-endOfLines >0){
    endOfLines += 1;
    lines[endOfLines] = newLine;
    return 0;
  }else{
    return -1;
  }
}

void Screen::setLine(int lineNumber, String text){
  lines[lineNumber+beginningDisplay] = text;
}





/* * * * * * * * FUNCTIONS TO SEND CONSTANTLY CHANGING VALUES TO SCREEN * * * * * * * */
String Screen::updateLine(int lineNumber){
  if(lines[lineNumber+beginningDisplay] == "TIME"){
    return String(getTime());
  }else if(lines[lineNumber+beginningDisplay] == "NOW"){
    return "NOW:   " + String(getTime());
  }else if(lines[lineNumber+beginningDisplay] == "DATE"){
    return getDate();
  }else{
    return lines[lineNumber+beginningDisplay];
  }
}

void Screen::sendLine(int lineNumber){
  Serial.print(updateLine(lineNumber));
}

void Screen::fullSend(){
  int i;
  for(i=0;i<4;i++){
    Serial.print("D");
    Serial.print(i+1);
    sendLine(i);
  }
}

String Screen::getTime(){
  DateTime now = rtcScreen.now();
  char temp[16];
  sprintf(temp, "%02d:%02d:%02d\n", now.hour(), now.minute(), now.second());//
//  Serial.print(temp);
//  return temp;
  String timeString(temp);
  return timeString;
//  return (String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second()) + "\n");
}
String Screen::getDate(){
  DateTime now = rtcScreen.now();
  String dateString = String(now.year()) + "/" + String(now.month()) + "/" + String(now.day()) + "\n";
  return dateString;
  
}
