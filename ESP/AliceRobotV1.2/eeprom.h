#include <Arduino.h>

typedef struct{
  String Distance;
  String OriginalTires;
  String AddedTires;
}PoleInfo;

typedef struct{
  PoleInfo Poles[10]; //At least 4 and no more than 10 poles will be required.
  String  StartTime;
  String StartDate;
  String RunTime;
  String TiresUsed;
  String TotalPoles;
}RunInfo;

void sendRunInfoToSPIFFS(RunInfo info);
void printRunInfoToSerial(RunInfo info);
void parseDataLineToRunInfo(String rawInfo, RunInfo *currentRunInfo);
void getLineStarts(int *lenLines, long *lineStarts);
int getLogLine(int lineNumber, String *line, long *lineStarts, int lenLines);
