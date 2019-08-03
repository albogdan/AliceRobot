#include <Wire.h>
#include "RTClib.h"

RTC_DS1307 rtc;
DateTime startTime;
DateTime endTime;
void setup() {
    Serial.begin(19200);
    if (! rtc.begin()) {
       Serial.println("Couldn't find RTC");
       while (1);
    }
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    startTime = rtc.now();
  // put your setup code here, to run once:
    char temp[16];
    sprintf(temp, "%02d:%02d:%02d", startTime.hour(), startTime.minute(), startTime.second());//
    String runStartTime(temp);
    Serial.println("Start" + runStartTime);

}

void loop() {
    endTime = rtc.now();
    TimeSpan runTime = endTime - startTime;
//    showTimeSpan("CURRENT TIME ELSPASE", runTime);
    char temp[16];
    sprintf(temp, "%02d:%02d", runTime.minutes(), runTime.seconds());//
    String runStartTime(temp);
    Serial.println(runStartTime);
//    
//    sprintf(temp, "%02d:%02d:%02d", endTime.hour(), endTime.minute(), endTime.second());//
//    runStartTime = temp;
//    Serial.println("End" + runStartTime);
//    Serial.println("UNIE" + endTime.unixtime());

    delay(500);
  // put your main code here, to run repeatedly:

}


void showTimeSpan(const char* txt, const TimeSpan& ts) {
    Serial.print(txt);
    Serial.print(" ");
    Serial.print(ts.days(), DEC);
    Serial.print(" days ");
    Serial.print(ts.hours(), DEC);
    Serial.print(" hours ");
    Serial.print(ts.minutes(), DEC);
    Serial.print(" minutes ");
    Serial.print(ts.seconds(), DEC);
    Serial.print(" seconds (");
    Serial.print(ts.totalseconds(), DEC);
    Serial.print(" total seconds)");
    Serial.println();
}
