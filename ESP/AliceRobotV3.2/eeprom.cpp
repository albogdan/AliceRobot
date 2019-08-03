#include <FS.h>   // Include the SPIFFS library
#include "eeprom.h"

int getLogLine(int lineNumber, String *line, long *lineStarts, int lenLines){
    File file = SPIFFS.open("/data.csv", "r");
    int i;

    if(lenLines<lineNumber){
      return -1;
    }
    file.seek(lineStarts[(lenLines-2)-lineNumber]); //lenLines is 1 too big b/c it registers the EOL too
    *line = file.readStringUntil('\n');
    file.close();
    return 0;
}

//Gets the location (byte number) of the start of each line in the SPIFF
void getLineStarts(int *lenLines, long *lineStarts){
    File file = SPIFFS.open("/data.csv", "r");
    String s;
    lineStarts [0] = 0;
    *lenLines = 1;
    while (file.position()<file.size()){
      s=file.readStringUntil('\n');
      lineStarts[*lenLines] = file.position();
      *lenLines+=1;
      yield();
    }
}

void sendRunInfoToSPIFFS(RunInfo info){
  String toAppend = "";
  toAppend = info.StartTime + "," + info.StartDate + "," + info.RunTime + "," + info.TiresUsed + "," + info.TotalPoles;
  if((info.TotalPoles).toInt() == 0 ){
    toAppend = toAppend + '\n';
  }
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

    File file = SPIFFS.open("/data.csv", "a");
    file.print(toAppend);
    file.close();
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
  Serial.println("got indexes");
  //Upload static values to structure
  currentRunInfo -> StartTime = rawInfo.substring(0, startTimeIndex);
  currentRunInfo -> StartDate = rawInfo.substring(startTimeIndex+1, startDateIndex);
  currentRunInfo -> RunTime = rawInfo.substring(startDateIndex+1, runTimeIndex);
  currentRunInfo -> TiresUsed = rawInfo.substring(runTimeIndex+1, tiresUsedIndex);
  currentRunInfo -> TotalPoles = (rawInfo.substring(tiresUsedIndex+1, totalPolesIndex));//.toInt();
  Serial.println("got substrings");
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
  Serial.println("loop done");
}
