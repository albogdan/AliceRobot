#include <RTClib.h>

#include "Screen.h"
#include <Wire.h>
#include <FS.h>   // Include the SPIFFS library
//#include <ESP8266WiFi.h>
//#include <WiFiClient.h>
#include <ESP8266WiFiMulti.h>
//#include <ESP8266mDNS.h> 
#include <ESP8266WebServer.h>

#include <AccelStepper.h>



#define MODE_HOME 1
#define MODE_RUNNING 2
#define MODE_LOGS 3
#define MODE_SPECIFIC_LOG 4

#define HALFSTEP 8
#define QUARTER_REVOLUTION 1019
#define HALF_REVOLUTION 2038
#define FULL_REVOLUTION 4076 //Note that 3 full revolutions after testing is almost exactly 1 conveyor length

// Motor pin definitions
#define motorPin1  13     // IN1 on the ULN2003 driver 1
#define motorPin2  12     // IN2 on the ULN2003 driver 1
#define motorPin3  14     // IN3 on the ULN2003 driver 1
#define motorPin4  2     // IN4 on the ULN2003 driver 1

#define motorPin5 7     //IN1
#define motorPin6 11    //IN2
#define motorPin7 8     //IN3
#define motorPin8 9     //IN4

AccelStepper stepperConveyor(HALFSTEP, motorPin1, motorPin3, motorPin2, motorPin4);
//AccelStepper stepper2(HALFSTEP, motorPin5, motorPin7, motorPin6, motorPin8);

void handleRoot();
void handleMotorFWD();
void handleMotorBKWD();
void handleMotorOFF();
void handleNotFound();
ESP8266WiFiMulti wifiMulti;     // Create an instance of the ESP8266WiFiMulti class, called 'wifiMulti'
ESP8266WebServer server(80); // Create a webserver object that listens for HTTP request on port 80


RTC_DS1307 rtc;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
bool stringComplete = false;
String inputString = "";

Screen homeScreen("TIME", "DATE", "1 - Start Run\n", "2 - View Logs\n", 0, rtc);
Screen runningScreen("Running...\n", "\n", "\n", "\n",0, rtc);
Screen emptyScreen("                \n","                \n","                \n","                \n",0,rtc);
Screen logScreen;
Screen specificLog;
Screen currentScreen;

int DeviceMode;
DateTime startTime;
DateTime endTime;
bool firstStart;
String characters = "0123456789";

RunInfo currentRunInfo;

void setup() {
    DeviceMode = MODE_HOME;
    firstStart = true; //Allows run to only start once, to prevent startTime from being overwritten

    inputString.reserve(200);
    
    Serial.begin(19200);
    delay(10);

    wifiMulti.addAP("JUPITER", "epvg8092");    
    while (wifiMulti.run() != WL_CONNECTED) { // Wait for the Wi-Fi to connect: scan for Wi-Fi networks, and connect to the strongest of the networks above
      delay(250);
//      Serial.print('.');
    }
//    Serial.println(WiFi.localIP());           // Send the IP address of the ESP8266 to PC
    
    if (! rtc.begin()) {
//      Serial.println("Couldn't find RTC");
      while (1);
    }
    if (! rtc.isrunning()) {
//      Serial.println("RTC is NOT running!");
    }
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
//    Serial.println("RTC Started");


    SPIFFS.begin(); //begin SPI Flash Files System
    delay(10);
//    Serial.println("SPIFFS started");

    server.on("/", HTTP_GET, handleRoot);     // Call the 'handleRoot' function when a client requests URI "/"
    server.on("/MOTORRIGHT", HTTP_POST, handleMotorRight);
    server.on("/MOTORLEFT", HTTP_POST, handleMotorLeft);
    server.on("/MOTORFWD", HTTP_POST, handleMotorFWD);  // Call the 'handleLED' function when a POST request is made to URI "/LED"
    server.on("/MOTORBKWD", HTTP_POST, handleMotorBKWD);  // Call the 'handleLED' function when a POST request is made to URI "/LED"
    server.on("/MOTOROFF", HTTP_POST, handleMotorOFF);  // Call the 'handleLED' function when a POST request is made to URI "/LED"
    server.on("/CONVEYOR", HTTP_POST, handleConveyor);
//    server.on("/RACK", HTTP_POST, handleRack);
    server.onNotFound(handleNotFound);        // When a client requests an unknown URI (i.e. something other than "/"), call function "handleNotFound"
    server.begin();

      
    currentScreen = homeScreen;
    emptyScreen.fullSend();

}

bool driving = false;
double totalDistance = 0.0;
void loop() {

/**********BEGIN IF STRING RECEIVED FROM UART**********/
    if(stringComplete){
        if(DeviceMode == MODE_RUNNING){
            if(inputString.substring(0,1) == "L"){
                totalDistance = (inputString.substring(1,inputString.length())).toFloat(); //Works perfectly when tested thru serial comm
            }
        }else if(DeviceMode == MODE_HOME){
            if(inputString == "K1\n" && firstStart){
                DeviceMode = MODE_RUNNING;
                firstStart = false;
                startTime = rtc.now();
                char temp[16];
                sprintf(temp, "%02d:%02d:%02d", startTime.hour(), startTime.minute(), startTime.second());//
                String runStartTime(temp);

                sprintf(temp, "%02d/%02d", startTime.day(), startTime.month());//
                String runStartDate(temp);

                //Initialize current run logs
                currentRunInfo.StartTime = runStartTime; //Add start time to logs
                currentRunInfo.StartDate = runStartDate; //Add start date to logs
                currentRunInfo.TotalPoles = "0"; //Add total poles to logs
                currentRunInfo.TiresUsed = "0";
                
                emptyScreen.fullSend();
                currentScreen = runningScreen;
                
                //Begin driving forward 
                Serial.println("MFB\n");
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
        currentScreen.fullSend();
    }
/**********END IF STRING RECEIVED FROM UART**********/



/*****************BEGIN PROGRAM LOGIC****************/    
    if(DeviceMode == MODE_RUNNING){

        //Check laser sensor if detects object (distance between 10 and 30 cm?)
            //If object(pole) detected then:
                //Stop motors
                ////Serial.println("MFF\n"); //might have to trip pin instead
                //Light up LED
                //Get the pole distance
                    
                //Move the sensor to the bottom of the pole to check for tires
                    //If distance decreases at specific point at top
                        //numberOfTires = 2
                        //move rack back to top
                    //Else if distance decreases at specific point at bottom
                        //numberOfTires = 1
                        //move rack back to top
                    //Else
                        //numberOfTires = 0
                        //move rack back to top
                //If 2 tires on pole then continue run
                //If 1 tire on pole
                    //If first pole
                        //Add a tire
                    //Else if (distCurrentPole-distPreviousPole) < 30
                        //continue run
                    //Else if  (distCurrentPole-distPreviousPole) > 30
                        //Add a tire
                //If 0 tires on pole
                    //If first pole
                        //Add a tire
                        //Add a tire
                    //Else if  (distCurrentPole-distPreviousPole) < 30
                        //Add a tire
                    //Else
                        //Add a tire
                        //Add a tire
            //If distance==4meteres or tiresLeft==0
                //go to start
                ////Serial.println("MBH\n"); //Move back home
                //record run information
                            
    }else if(DeviceMode == MODE_HOME){
        currentScreen.fullSend();
        delay(250);
    }

/******************END PROGRAM LOGIC****************/


/**********BEGIN CHECKING SERIAL AVAILABLE**********/
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
/**********END CHECKING SERIAL AVAILABLE**********/

    yield();
    server.handleClient();

}



/*
 * ------------WIFI FUNCTIONS START----------------------
 */
void handleRoot(){
  server.send(200, "text/html", "<form action=\"/MOTORFWD\" method=\"POST\"><input type=\"submit\" style=\"width:250px;height:100px;\" value=\"Move Forward\"></form><form action=\"/MOTORBKWD\" method=\"POST\"><input type=\"submit\" style=\"width:250px;height:100px;\" value=\"Move Backward\"></form><form action=\"/MOTORLEFT\" method=\"POST\"><input type=\"submit\" style=\"width:250px;height:100px;\" value=\"Move Left\"></form><form action=\"/MOTORRIGHT\" method=\"POST\"><input type=\"submit\" style=\"width:250px;height:100px;\" value=\"Move Right\"></form><form action=\"/MOTOROFF\" method=\"POST\"><input type=\"submit\" style=\"width:250px;height:100px;\" value=\"Turn Off\"></form> <form action=\"/CONVEYOR\" method=\"POST\"><input type=\"submit\" style=\"width:250px;height:100px;\" value=\"Test Conveyor\"></form><form action=\"/RACK\" method=\"POST\"><input type=\"submit\" style=\"width:250px;height:100px;\" value=\"Test Rack\"></form>");
}


void handleNotFound(){

    server.send(404, "text/plain", "404: Not found"); // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
}
void handleConveyor(){
    stepperConveyor.setMaxSpeed(2000.0);
    stepperConveyor.setAcceleration(100.0);
    stepperConveyor.setSpeed(700);
    stepperConveyor.moveTo(FULL_REVOLUTION);

    server.sendHeader("Location","/");        // Add a header to respond with a new location for the browser to go to the home page again
    server.send(303);                         // Send it back to the browser with an HTTP status 303 (See Other) to redirect
    while(stepperConveyor.distanceToGo() != 0 ){
        stepperConveyor.run();
        yield();
    }
  
}

//void handleRack(){
//  stepper2.setMaxSpeed(2000.0);
//  stepper2.setAcceleration(100.0);
//  stepper2.setSpeed(700);
//  stepper2.moveTo(HALF_REVOLUTION);
//
//  server.sendHeader("Location","/");        // Add a header to respond with a new location for the browser to go to the home page again
//  server.send(303);                         // Send it back to the browser with an HTTP status 303 (See Other) to redirect
//  while(stepper2.distanceToGo() != 0 ){
//    stepper2.run();
//  }
//  
//}

void handleMotorFWD(){
    Serial.write("MFB\n");
    server.sendHeader("Location","/");        // Add a header to respond with a new location for the browser to go to the home page again
    server.send(303);                         // Send it back to the browser with an HTTP status 303 (See Other) to redirect
}
void handleMotorLeft(){
    Serial.write("MSL\n");
    server.sendHeader("Location","/");        // Add a header to respond with a new location for the browser to go to the home page again
    server.send(303);                         // Send it back to the browser with an HTTP status 303 (See Other) to redirect
}
void handleMotorRight(){
    Serial.write("MSR\n");
    server.sendHeader("Location","/");        // Add a header to respond with a new location for the browser to go to the home page again
    server.send(303);                         // Send it back to the browser with an HTTP status 303 (See Other) to redirect
}
void handleMotorBKWD(){
    Serial.write("MBB\n");
    server.sendHeader("Location","/");        // Add a header to respond with a new location for the browser to go to the home page again
    server.send(303);                         // Send it back to the browser with an HTTP status 303 (See Other) to redirect  
}
void handleMotorOFF(){
    Serial.write("MFF\n");
    server.sendHeader("Location","/");        // Add a header to respond with a new location for the browser to go to the home page again
    server.send(303);                         // Send it back to the browser with an HTTP status 303 (See Other) to redirect
}

/*
 * ------------WIFI FUNCTIONS END----------------------
 */
