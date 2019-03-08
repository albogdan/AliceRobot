#include <FS.h>   // Include the SPIFFS library
#include "eeprom.h"


bool stringComplete = false;
String inputString = "";

void setup() {
  Serial.begin(115200); //begin serial
  delay(10);
  Serial.println("Begin");

  inputString.reserve(200);

  SPIFFS.begin(); //begin SPI Flash Files System
  Serial.println("SPIFFS started");
}

void loop() {
  if(Serial.available()){
    Serial.println("SERIAL AVAILABLE");
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
  if (stringComplete) {
    Serial.println("Command received");
    stringComplete = false;
//    Dir dir = SPIFFS.openDir("/");
//    String str = "";
//    while(dir.next()){
//        str += dir.fileName();
//        str += " / ";
//        str += dir.fileSize();
//        str += "\r\n";
//    }
//    Serial.print(str);
    File file = SPIFFS.open("/data.csv", "r"); //Open file read only
    if (!file) {
      Serial.println("File opening failed");
    }

    int i;
    for(i=0; i<3;i++){
      String s;
      getLogLine(2*i, &s);
      Serial.println(s);
    }

//
//    Serial.println("Line 1:");
//    String s = file.readStringUntil('\n');
//    Serial.println(s);
//    RunInfo currentRunInfo;
//    parseDataLineToRunInfo(s, &currentRunInfo);
//    printRunInfoToSerial(currentRunInfo);
//
//
//    Serial.println("Line 2:");
//    s = file.readStringUntil('\n');
//    Serial.println(s);
//    parseDataLineToRunInfo(s, &currentRunInfo);
//    printRunInfoToSerial(currentRunInfo);
//
//
//    Serial.println("Line 3:");
//    s = file.readStringUntil('\n');
//    Serial.println(s);
//    parseDataLineToRunInfo(s, &currentRunInfo);
//    printRunInfoToSerial(currentRunInfo);
//
    file.close();
//    sendRunInfoToSPIFFS(currentRunInfo);
  }

}
//StarTime,StartDate,Runtime,TiresUsed,TotalPoles,DistanceN,OriginalTiresN,AddedTiresN
