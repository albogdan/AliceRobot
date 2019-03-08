#include <FS.h>   // Include the SPIFFS library
#include "eeprom.h"

int getLogLine(int lineNumber, String *line){
    File file = SPIFFS.open("/data.csv", "r");
    int lenLines, i;
    long lineStarts[100];
    String s;
    lineStarts[0] = 0;
    lenLines = 1;

    while (file.position()<file.size()){
      s=file.readStringUntil('\n');
      lineStarts[lenLines] = file.position();
      lenLines+=1;
    }
    if(lenLines<lineNumber){
      return -1;
    }
    Serial.println("Length:" + String(lenLines));
    file.seek(lineStarts[(lenLines-2)-lineNumber]); //lenLines is 1 too big b/c it registers the EOL too
    *line = file.readStringUntil('\n');
    file.close();
    return 0;
}

void sendRunInfoToSPIFFS(RunInfo info){
  String toAppend = "";
  toAppend = info.StartTime + "," + info.StartDate + "," + info.RunTime + "," + info.TiresUsed + "," + info.TotalPoles;
  int i;
  for(i=0; i< (info.TotalPoles).toInt(); i++){
    if(i != (info.TotalPoles).toInt()){
      toAppend = toAppend + ",";
    }
    toAppend = toAppend + (info.Poles[i]).Distance + "," + (info.Poles[i]).OriginalTires + "," + (info.Poles[i]).AddedTires;
    if(i == (info.TotalPoles).toInt()-1){
      toAppend = toAppend + '\n';
    }
  }
//  Serial.println("INFO TO SEND TO SPIFFS:");
//  Serial.print(toAppend);

//  Serial.println("READING FROM SPIFFS:");

    File file = SPIFFS.open("/data.csv", "a");
    file.print(toAppend);
    file.close();
//    String s;
//    Serial.println("File size: " + String(file.size()));
//    Serial.println("Pos: " + String(file.position()));
//    while (file.position()<file.size()){
//      s=file.readStringUntil('\n');
//      Serial.println("Pos: " + String(file.position()));    
//    } 

//  File file = SPIFFS.open("/data.csv", "r"); //Open file read mode
//  String previousFile;
//  while (file.available()){
//    previousFile += char(file.read());
//  }
////  Serial.println(previousFile);
//  file.close();
//
////  Serial.println("WRITING NEW SPIFFS");
//  File fileNew = SPIFFS.open("/data.csv", "w");
//  fileNew.print(toAppend);
//  fileNew.print(previousFile);
//  fileNew.close();
//
//  Serial.println("NEW FILE:");
//  File fileNew2 = SPIFFS.open("/data.csv", "r");
//  String string2;
//  while (fileNew2.available()){
//    string2 += char(fileNew2.read());
//  }
//  Serial.println(string2);
//  file.close();
//  

}

void printRunInfoToSerial(RunInfo info){
  Serial.println("Start Time:" + info.StartTime);
  Serial.println("Start Date:" + info.StartDate);
  Serial.println("Runtime:" +  info.RunTime);
  Serial.println("Tires Used:" + info.TiresUsed);
  Serial.println("Total Poles:"+ info.TotalPoles);
  int i;
  for(i=0; i< (info.TotalPoles).toInt(); i++){
    Serial.println("Distance:"+(info.Poles[i]).Distance);
    Serial.println("OriginalTires:"+(info.Poles[i]).OriginalTires);
    Serial.println("AddedTires:"+(info.Poles[i]).AddedTires);
  }
}


void parseDataLineToRunInfo(String rawInfo, RunInfo *currentRunInfo){
  //Determine indexes of static values
  int startTimeIndex = rawInfo.indexOf(',');
  int startDateIndex = rawInfo.indexOf(',', startTimeIndex+1);
  int runTimeIndex = rawInfo.indexOf(',', startDateIndex+1);
  int tiresUsedIndex = rawInfo.indexOf(',',runTimeIndex+1);
  int totalPolesIndex = rawInfo.indexOf(',', tiresUsedIndex+1);
  
  //Upload static values to structure
  currentRunInfo -> StartTime = rawInfo.substring(0, startTimeIndex);
  currentRunInfo -> StartDate = rawInfo.substring(startTimeIndex+1, startDateIndex);
  currentRunInfo -> RunTime = rawInfo.substring(startDateIndex+1, runTimeIndex);
  currentRunInfo -> TiresUsed = rawInfo.substring(runTimeIndex+1, tiresUsedIndex);
  currentRunInfo -> TotalPoles = (rawInfo.substring(tiresUsedIndex+1, totalPolesIndex));//.toInt();

  //Determine and upload each pole to structure
  int i;
  int previousStartIndex = totalPolesIndex;
  for(i=0; i< (currentRunInfo->TotalPoles).toInt(); i++){
    int distanceIndex = rawInfo.indexOf(',', previousStartIndex+1);
    int originalTiresIndex = rawInfo.indexOf(',', distanceIndex+1);
    int addedTiresIndex = rawInfo.indexOf(',', originalTiresIndex+1);
    (currentRunInfo -> Poles[i]).Distance = rawInfo.substring(previousStartIndex+1, distanceIndex);
    (currentRunInfo -> Poles[i]).OriginalTires = rawInfo.substring(distanceIndex+1, originalTiresIndex);
    (currentRunInfo -> Poles[i]).AddedTires = rawInfo.substring(originalTiresIndex+1, addedTiresIndex);
    previousStartIndex = addedTiresIndex;
  }
}
