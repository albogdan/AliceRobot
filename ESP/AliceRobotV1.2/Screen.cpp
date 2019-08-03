#include <WString.h>

#include "Screen.h"
//using namespace N;



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
Screen::Screen(){
  beginningDisplay = 0;
  endOfLines = 0;  
}


void Screen::moveScreenDown(){
    if(beginningDisplay+1+4<=endOfLines){
        beginningDisplay+=1;
    }
}

void Screen::moveScreenUp(){
    if(beginningDisplay >0){
        beginningDisplay -=1;
    }
}


//MIGHT RUN INTO PROBLEMS WITH MEMORY IF LOAD ALL INTO MEMORY, GONNA MAX IT OUT AT 10 FOR NOW
void Screen::loadLogs(){
    int i, returnVal;
    int maxRecords;
    String rawInfo;
    RunInfo info;
    getLineStarts(&lenLines, lineStarts);
    delay(10);
    Serial.println("lenLines:: " + String(lenLines));
    delay(10);
    if(lenLines-1 < 10){
        maxRecords = lenLines -1;
    }else{
        maxRecords = 10;
    }
    for(i=0;i<maxRecords;i++){
        returnVal = getLogLine(i, &rawInfo, lineStarts, lenLines);
        if(returnVal != -1){
            parseDataLineToRunInfo(rawInfo, &info);
        }
        String lineToAdd;
        lineToAdd = String(i+1) + "-" + info.StartDate + "," + info.StartTime + '\n';
        addLine(lineToAdd);
    }
}

void Screen::viewSpecificLog(int location, Screen *specificLog){
    int returnVal;
    String rawInfo;
    RunInfo info;
    int i;
    Serial.println("checking if: "+ String(location));
    Serial.println("len: " + String(lenLines-1));
    if(location<lenLines-1){
        Serial.println("Getting log line");
        returnVal = getLogLine(location, &rawInfo, lineStarts, lenLines);
        Serial.println(rawInfo);
    }
    parseDataLineToRunInfo(rawInfo, &info);
    String lineToAdd;
    lineToAdd = "Date:" + info.StartDate + "\n";
    specificLog->addLine(lineToAdd);
    lineToAdd = "Start:" + info.StartTime + "\n";
    specificLog->addLine(lineToAdd);
    lineToAdd = "Run:" + info.RunTime + "\n";
    specificLog->addLine(lineToAdd);
    lineToAdd = "Tires Used:" + info.TiresUsed + "\n";
    specificLog->addLine(lineToAdd);
    lineToAdd = "Total Poles:" + info.TotalPoles + "\n";
    specificLog->addLine(lineToAdd);
    for(i=0; i< (info.TotalPoles).toInt(); i++){
        lineToAdd = "--POLE " + String(i+1) + "--\n";
        specificLog->addLine(lineToAdd);
        lineToAdd = "D:"+(info.Poles[i]).Distance + "mm\n";
        specificLog->addLine(lineToAdd);
        lineToAdd = "ST:"+(info.Poles[i]).OriginalTires + "  AT:" +(info.Poles[i]).AddedTires.substring(0,1) +"\n";
        specificLog->addLine(lineToAdd);
//        lineToAdd = "Added Tires:"+(info.Poles[i]).AddedTires + "\n";
//        specificLog->addLine(lineToAdd);
    }

}




/* ADD LINE TO THE LIST OF LINES
 * newLine is the string you wish to add
 */
int Screen::addLine(String newLine){
  if(SIZE_OF_LINES-endOfLines >0){
    lines[endOfLines] = newLine;
    endOfLines += 1;
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
  String dateString = String(now.year()) + "/" + String(now.month()) + "/" + String(now.day()) + "    \n";
  return dateString;
  
}
