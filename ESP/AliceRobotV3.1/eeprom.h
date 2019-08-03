#include <Arduino.h>
#include "variables.h"

void sendRunInfoToSPIFFS(RunInfo info);
void printRunInfoToSerial(RunInfo info);
void parseDataLineToRunInfo(String rawInfo, RunInfo *currentRunInfo);
void getLineStarts(int *lenLines, long *lineStarts);
int getLogLine(int lineNumber, String *line, long *lineStarts, int lenLines);
