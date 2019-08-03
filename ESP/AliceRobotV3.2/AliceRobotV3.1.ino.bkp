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
#include "Adafruit_VL53L0X.h"



#define MODE_HOME 1
#define MODE_RUNNING 2
#define MODE_LOGS 3
#define MODE_SPECIFIC_LOG 4

#define HALFSTEP 8
#define FULLSTEP 4
#define QUARTER_REVOLUTION 1019
#define HALF_REVOLUTION 2038
#define FULL_REVOLUTION 4076 //Note that 3 full revolutions after testing is almost exactly 1 conveyor length


#define CONVEYOR_START_TIRE_WIDTH 1960
#define CONVEYOR_TIRE_WIDTH 1908

#define SENSOR_TOP_STEPPER -1350//-2700
#define SENSOR_BOTTOM_STEPPER -2700//-5400
#define HOME_STEPPER 0

// Motor pin definitions
//#define motorPin1  13     // IN1 on the ULN2003 driver 1
//#define motorPin2  12     // IN2 on the ULN2003 driver 1
//#define motorPin3  14     // IN3 on the ULN2003 driver 1
//#define motorPin4  2     // IN4 on the ULN2003 driver 1

#define motorPin1  D7     // IN1 on the ULN2003 driver 1
#define motorPin2  D6     // IN2 on the ULN2003 driver 1
#define motorPin3  D5     // IN3 on the ULN2003 driver 1
#define motorPin4  D4     // IN4 on the ULN2003 driver 1

#define motorPin5 D0     //IN1
#define motorPin6 D3    //IN2
#define motorPin7 D8     //IN3
#define motorPin8 10     //IN4 //GPIO 10 SEEMS TO WORK??


//Laser sensor
Adafruit_VL53L0X lox = Adafruit_VL53L0X();


void handleRoot();
void handleMotorFWD();
void handleMotorBKWD();
void handleMotorOFF();
void handleNotFound();
void handleDownloadLogs();
ESP8266WiFiMulti wifiMulti;     // Create an instance of the ESP8266WiFiMulti class, called 'wifiMulti'
ESP8266WebServer server(80); // Create a webserver object that listens for HTTP request on port 80


RTC_DS1307 rtc;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
bool stringComplete = false;
String inputString = "";

const long int conveyorTireLocationsStart[8] = {-775, -1625, -2475, -3325, -4175, -5025, -5875, -6725};
const long int conveyorTireLocationsReload[7] = {-975, -1915, -2855, -3795, -4735, -5675, -6615};
const long int* conveyorTireLocations = conveyorTireLocationsStart;

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
AccelStepper stepperConveyor(FULLSTEP, motorPin1, motorPin3, motorPin2, motorPin4);
//AccelStepper stepperConveyor(HALFSTEP, motorPin1, motorPin3, motorPin2, motorPin4);
AccelStepper stepperSensor(FULLSTEP, motorPin5, motorPin7, motorPin6, motorPin8);

void setup() {
    DeviceMode = MODE_HOME;
    firstStart = true; //Allows run to only start once, to prevent startTime from being overwritten

    inputString.reserve(200);
    
    Serial.begin(19200);
    delay(10);

    Serial.println("Serial started");
    wifiMulti.addAP("JUPITER", "epvg8092");   
    wifiMulti.addAP("Shash","shashwat"); 
    while (wifiMulti.run() != WL_CONNECTED) { // Wait for the Wi-Fi to connect: scan for Wi-Fi networks, and connect to the strongest of the networks above
      delay(250);
      Serial.print('.');
    }
    Serial.println(WiFi.localIP());           // Send the IP address of the ESP8266 to PC
    
    if (! rtc.begin()) {
//      Serial.println("Couldn't find RTC");
//      while (1);
    }
    if (! rtc.isrunning()) {
//      Serial.println("RTC is NOT running!");
    }
//    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
//DateTime::DateTime (uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec)

//    rtc.adjust(DateTime(2019, 4, 7, 16, 46, 0));
    Serial.println("RTC Started");


    if (!lox.begin()) {
//        Serial.println(F("Failed to boot VL53L0X"));
//        while(1);
    }


    SPIFFS.begin(); //begin SPI Flash Files System
    delay(10);
    Serial.println("SPIFFS started");

    server.on("/", HTTP_GET, handleRoot);     // Call the 'handleRoot' function when a client requests URI "/"
    server.on("/MOTORRIGHT", HTTP_POST, handleMotorRight);
    server.on("/MOTORLEFT", HTTP_POST, handleMotorLeft);
    server.on("/MOTORFWD", HTTP_POST, handleMotorFWD);  // Call the 'handleLED' function when a POST request is made to URI "/LED"
    server.on("/MOTORBKWD", HTTP_POST, handleMotorBKWD);  // Call the 'handleLED' function when a POST request is made to URI "/LED"
    server.on("/MOTOROFF", HTTP_POST, handleMotorOFF);  // Call the 'handleLED' function when a POST request is made to URI "/LED"
    server.on("/CONVEYOR", HTTP_POST, handleConveyor);
    server.on("/DOWNLOAD", HTTP_POST, handleDownloadLogs);
//    server.on("/RACK", HTTP_POST, handleRack);
    server.onNotFound(handleNotFound);        // When a client requests an unknown URI (i.e. something other than "/"), call function "handleNotFound"
    
    server.serveStatic("/data.csv", SPIFFS, "/data.csv");
    
    server.begin();

    stepperSensor.setMaxSpeed(3000.0);
    stepperSensor.setAcceleration(200.0);
    stepperSensor.setSpeed(2950);
    stepperConveyor.setMaxSpeed(2000.0);
    stepperConveyor.setAcceleration(200.0);
    stepperConveyor.setSpeed(1950);
      
    currentScreen = homeScreen;
    emptyScreen.fullSend();
    Serial.println("Init complete");

}

bool driving = false;
double totalDistance = 0.0;
int currentPole = 0;
bool sensorStepperMoving = false;
bool conveyorStepperMoving = false;
int tiresLeft = 8;
long int conveyorPosition = 0;
int currentTire = 0;
int distanceLoopCounter = 0;
bool runComplete = false;
void loop() {

/**********BEGIN IF STRING RECEIVED FROM UART**********/
    if(stringComplete){
        if(DeviceMode == MODE_RUNNING){
            if(inputString.substring(0,1) == "L"){
                totalDistance = (inputString.substring(1,inputString.length())).toFloat(); //Works perfectly when tested thru serial comm
                //Serial.print("D3D:" + String(totalDistance,2) + '\n');
                
                (currentRunInfo.Poles[currentPole]).Distance = String(totalDistance);
            }else if(inputString == "K1\n" && runComplete){
                DeviceMode = MODE_LOGS;
                logScreen.loadLogs();
                currentScreen = logScreen;
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
            if(inputString == "KC\n"){ //If pressed down button
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
            if(inputString == "KC\n"){ //If pressed down button
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
  //   bool qVal = false;
    if(DeviceMode == MODE_RUNNING){

        if(distanceLoopCounter == 40){
            //Serial.print("D4Di:" + String(totalDistance,2) + '\n');

            distanceLoopCounter=0;
            Serial.write("Q\n");
          //  qVal = true;
        }
        distanceLoopCounter+=1;

        VL53L0X_RangingMeasurementData_t measureStart, measureTire, measureSideToSide;
        lox.rangingTest(&measureStart,false); //pass in 'true' to get debug data printout
        bool poleDetected = false;
        
        //**********Check laser sensor if detects object (distance between 100 and 500 mm)**********
        if(measureStart.RangeStatus!=4 && (measureStart.RangeMilliMeter > 100 && measureStart.RangeMilliMeter < 500)){
            poleDetected = true;
            currentRunInfo.TotalPoles = String((currentRunInfo.TotalPoles.toInt())+1);
        }
        //If object(pole) detected then:
        if(poleDetected){
            //Stop motors
            Serial.write("MFF\n"); //might have to trip pin instead

            //**********Delay for stop between moving fwd and going sideways **********
            delay(800);
            //Light up LED
            //*********************
            //Get the pole distance (SEND character Q to PIC)
            Serial.write("Q\n");

            while(1){
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
                if(stringComplete){
                    if(inputString.substring(0,1) == "L"){
                        totalDistance = (inputString.substring(1,inputString.length())).toFloat(); //Works perfectly when tested thru serial comm
                        (currentRunInfo.Poles[currentPole]).Distance = String(totalDistance);
                        Serial.print("D3D:" + String(totalDistance) + '\n');
                    }
                    inputString = "";
                    stringComplete = false;
                    break;
                }   
                yield();         
            }


            
            int tiresToAdd = 0;

            //Move the robot closer to the pole
            //Turn on the motors to more to the right
            Serial.write("MSR\n");
            //Delay an amount, might have to make it based on the distance of the robot from the measurement of the laser sensor
            //Turn off motor move when robot is close enough
            while(1){
                int i;
                int movingAvg;
                 //pass in 'true' to get debug data printout
                lox.rangingTest(&measureSideToSide,false);
                if(measureSideToSide.RangeStatus!=4){
                    movingAvg = measureSideToSide.RangeMilliMeter;
                }
                Serial.print("D4S:" + String(measureSideToSide.RangeMilliMeter)+ '\n');
                
                //**********Distance to move to the right (120mm)**********
                
                
                for(i=1;i<=25; i++){
                    lox.rangingTest(&measureSideToSide,false);
                    if(measureSideToSide.RangeStatus!=4){
                        movingAvg = (movingAvg*i+ measureSideToSide.RangeMilliMeter)/(i+1);
                    }
                }
                if(movingAvg < 57){
                    Serial.write("MFF\n");
                    delay(100);
                    Serial.write("MFF\n");
                    delay(10);
                    break;
                }
//                int j = 0;
//                while(1){
//                    lox.rangingTest(&measureSideToSide,false);
//                    if(measureSideToSide.RangeStatus!=4 && measureSideToSide.RangeMilliMeter < 57){
//                        Serial.write("MFF\n");
//                        delay(100);
//                        Serial.write("MFF\n");
//                        delay(10);
//                        break;
//                    }
//                    if(j==400){
//                        break;
//                    }
//                    j+=1;
//                    delay(1);
//                    yield();
//                }
                delay(200);
                Serial.write("MFF\n");
                delay(300);
                Serial.write("MSR\n");
                delay(10);
                yield();
            }


            //Re-get start distance measurement for tire count, since moved in and will be different now
            lox.rangingTest(&measureStart,false); //pass in 'true' to get debug data printout
            
            //Move the sensor to the first tire down on the pole to check for 2 tires
            stepperSensor.moveTo(SENSOR_TOP_STEPPER);
            while(stepperSensor.distanceToGo() != 0 ){
                stepperSensor.run();
                yield();
            }
            //If distance decreases at specific point at top
            int tiresOnPole = 0;
            lox.rangingTest(&measureTire, false);
            if(measureStart.RangeMilliMeter - measureTire.RangeMilliMeter >15){
                currentRunInfo.Poles[currentPole].OriginalTires = String(2);
                currentRunInfo.Poles[currentPole].AddedTires = String(0);
                //currentRunInfo.TiresUsed = String((currentRunInfo.TiresUsed.toInt()) + 0);
                Serial.print("D2Tires:2\n");
 /*******NOTE THERE WILL BE A BUG - HAVE TO ALSO CONSIDER NUMBER OF TIRES CURRENTLY AVAILABLE TO DEPLOY******/
                tiresToAdd = 0;
                //numberOfTires = 2
                //move rack back to top ******
            }else{
                stepperSensor.moveTo(SENSOR_BOTTOM_STEPPER);
                while(stepperSensor.distanceToGo() != 0 ){
                    stepperSensor.run();
                    yield();
                }
                lox.rangingTest(&measureTire, false);
                //Else if distance decreases at specific point at bottom
                if(measureStart.RangeMilliMeter - measureTire.RangeMilliMeter >15){
                    currentRunInfo.Poles[currentPole].OriginalTires = String(1);
                    currentRunInfo.Poles[currentPole].AddedTires = String(1);
                    //currentRunInfo.TiresUsed = String((currentRunInfo.TiresUsed.toInt()) + 1);
                    tiresToAdd = 1;        
                    Serial.print("D2Tires:1\n");            
                    //numberOfTires = 1
                    //move rack back to top ******
                }else{//Else - there's no tires already on the pole
                    currentRunInfo.Poles[currentPole].OriginalTires = String(0);
                    currentRunInfo.Poles[currentPole].AddedTires = String(2);
                    //currentRunInfo.TiresUsed = String((currentRunInfo.TiresUsed.toInt()) + 2);
                    tiresToAdd = 2;
                    Serial.print("D2Tires:0\n");
                    //numberOfTires = 0
                    //move rack back to top *******
                }
            }
//            stepperSensor.moveTo(HOME_STEPPER);
//            sensorStepperMoving = true;
            
            
            //If 0 tires to add then continue run
            //If 1 tire to add on pole
            if(tiresToAdd == 1){
                //If first pole
                if(currentPole == 0){
                    //Add a tire
                    conveyorPosition = conveyorTireLocations[currentTire];//CONVEYOR_START_TIRE_WIDTH;
                    currentTire +=1;
                    stepperConveyor.moveTo(conveyorPosition);
                    while(stepperConveyor.distanceToGo() != 0 ){
                        stepperConveyor.run();
                        yield();
                    }
                    //Subtract from total tires, add to tires used
                    tiresLeft -=1;
                    currentRunInfo.TiresUsed = String(currentRunInfo.TiresUsed.toInt() +1);
                }else{
                    double distPreviousPole = (currentRunInfo.Poles[currentPole-1]).Distance.toFloat();
                    //if (distCurrentPole-distPreviousPole) > 30
                    if(totalDistance - distPreviousPole > 300){
                        //Add a tire
                        //conveyorPosition += CONVEYOR_TIRE_WIDTH;
                        conveyorPosition = conveyorTireLocations[currentTire];//CONVEYOR_START_TIRE_WIDTH;
                        currentTire +=1;

                        stepperConveyor.moveTo(conveyorPosition);
                        while(stepperConveyor.distanceToGo() != 0 ){
                            stepperConveyor.run();
                            yield();
                        }
                        //Subtract from total tires, add to tires used
                        tiresLeft -=1;
                        currentRunInfo.TiresUsed = String(currentRunInfo.TiresUsed.toInt() +1);
                    }
                    //Else continue run
                }
            

            //If 2 tires to add on pole
            }else if(tiresToAdd == 2){
                //If first pole
                if(currentPole == 0){
                    //Add a tire
                    //Add a tire
                    //conveyorPosition = CONVEYOR_START_TIRE_WIDTH + CONVEYOR_TIRE_WIDTH;
                    conveyorPosition = conveyorTireLocations[currentTire+1];//CONVEYOR_START_TIRE_WIDTH;
                    currentTire +=2;

                    stepperConveyor.moveTo(conveyorPosition);
                    while(stepperConveyor.distanceToGo() != 0 ){
                        stepperConveyor.run();
                        yield();
                    }
                    //Subtract from total tires, add to tires used
                    tiresLeft -=2;
                    currentRunInfo.TiresUsed = String(currentRunInfo.TiresUsed.toInt() +2);
                }else{
                    double distPreviousPole = (currentRunInfo.Poles[currentPole-1]).Distance.toFloat();
                    
                    //if  (distCurrentPole-distPreviousPole) < 30 add a tire
                    if(totalDistance - distPreviousPole < 300){
                        //Add a tire
                        //conveyorPosition += CONVEYOR_TIRE_WIDTH;
                        conveyorPosition = conveyorTireLocations[currentTire];//CONVEYOR_START_TIRE_WIDTH;
                        currentTire +=1;                
                        stepperConveyor.moveTo(conveyorPosition);
                        while(stepperConveyor.distanceToGo() != 0 ){
                            stepperConveyor.run();
                            yield();
                        }    
                        //Subtract from total tires, add to tires used
                        tiresLeft -=1;
                        currentRunInfo.TiresUsed = String(currentRunInfo.TiresUsed.toInt() +1);
    
                    //else add two tires
                    }else{
                        //Add a tire
                        //Add a tire
                        //conveyorPosition += 2*CONVEYOR_TIRE_WIDTH;
                        if(tiresLeft < 2){
                            conveyorPosition = conveyorTireLocations[currentTire];
                            currentTire +=1;
                            stepperConveyor.moveTo(conveyorPosition);
                            while(stepperConveyor.distanceToGo() != 0 ){
                                stepperConveyor.run();
                                yield();
                            }
                            tiresLeft -=1;
                            currentRunInfo.TiresUsed = String(currentRunInfo.TiresUsed.toInt() +1);

                            stepperConveyor.moveTo(HOME_STEPPER-100);
                            while(stepperConveyor.distanceToGo() != 0 ){
                                stepperConveyor.run();
                                yield();
                            }
                            currentTire = 0;
                            tiresLeft = 7;
                            conveyorTireLocations = conveyorTireLocationsReload;

                            
                            conveyorPosition = conveyorTireLocations[currentTire];
                            currentTire +=1;
                            stepperConveyor.moveTo(conveyorPosition);
                            while(stepperConveyor.distanceToGo() != 0 ){
                                stepperConveyor.run();
                                yield();
                            }
                            
                            tiresLeft -=1;
                            currentRunInfo.TiresUsed = String(currentRunInfo.TiresUsed.toInt() +1);
                        }else{
                            conveyorPosition = conveyorTireLocations[currentTire+1];//CONVEYOR_START_TIRE_WIDTH;
                            currentTire +=2;
                            
                            stepperConveyor.moveTo(conveyorPosition);
                            while(stepperConveyor.distanceToGo() != 0 ){
                                stepperConveyor.run();
                                yield();
                            }    
                            //Subtract from total tires, add to tires used
                            tiresLeft -=2;
                            currentRunInfo.TiresUsed = String(currentRunInfo.TiresUsed.toInt() +2);
                        }
                    }
                }
            }
            stepperSensor.moveTo(HOME_STEPPER);
            while(stepperSensor.distanceToGo() != 0 ){
                stepperSensor.run();
                yield();
            }

            if(tiresLeft == 0){
                stepperConveyor.moveTo(HOME_STEPPER-100);
                while(stepperConveyor.distanceToGo() != 0 ){
                    stepperConveyor.run();
                    yield();
                }
                currentTire = 0;
                tiresLeft = 7;
                conveyorTireLocations = conveyorTireLocationsReload;
            }
            
            //Move the robot away from to the pole
            //Turn on the motors to more to the left
            Serial.write("MSL\n");

            //Delay an amount, might have to make it based on the distance of the robot from the measurement of the laser sensor
            //Turn off motor move when robot is far enough (>25cm away)
            while(1){
                int i;
                int movingAvg;
                 //pass in 'true' to get debug data printout
                lox.rangingTest(&measureSideToSide,false);
                if(measureSideToSide.RangeStatus!=4){
                    movingAvg = measureSideToSide.RangeMilliMeter;
                }
                Serial.print("D4S:" + String(measureSideToSide.RangeMilliMeter)+ '\n');
                
                //**********Distance to move to the right (120mm)**********
                
                
                for(i=1;i<=25; i++){
                    lox.rangingTest(&measureSideToSide,false);
                    if(measureSideToSide.RangeStatus!=4){
                        movingAvg = (movingAvg*i+ measureSideToSide.RangeMilliMeter)/(i+1);
                    }
                }
                if(movingAvg > 180){
                    Serial.write("MFF\n");
                    delay(100);
                    Serial.write("MFF\n");
                    delay(10);
                    break;
                }
                
                yield();
                delay(200);
                Serial.write("MFF\n");
                delay(300);
                Serial.write("MSL\n");
                delay(10);
                yield();
            }

            //Begin moving forward once again
            Serial.write("MFB\n");
            delay(1000);
            currentPole +=1;
        }
        //If distance==4meteres or tiresLeft==0
        if(totalDistance >= 2000.0 || tiresLeft <= 0){
            //go to start
            Serial.write("MFF\n");
            delay(100);
            Serial.write("MBB\n"); //Move back home
            long int i = 0;
            while(1){
                if(i == 10000){
                    i=0;
                    Serial.write("Q\n");
                }
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
                if(stringComplete){
                    if(inputString.substring(0,1) == "L"){
                        totalDistance = (inputString.substring(1,inputString.length())).toFloat(); //Works perfectly when tested thru serial comm
                    }
                    inputString = "";
                    stringComplete = false;
                }
                if(totalDistance < 0){
                    Serial.write("MFF\n");
                    break;
                }
                i++;
                yield();
            }
            //record run information                
            endTime = rtc.now();
            TimeSpan runTime  = endTime - startTime;
            char temp[16];
            sprintf(temp, "%02d:%02d", runTime.minutes(), runTime.seconds());//
            currentRunInfo.RunTime = String(temp);

            //Upload data to SPIFFS
            sendRunInfoToSPIFFS(currentRunInfo);
            emptyScreen.fullSend();
            Serial.print("D2Run Complete!\n");
            Serial.print("D31 - View Logs\n");
            runComplete = true;
        }
//        if(sensorStepperMoving){
//            stepperSensor.run();
//            if(stepperSensor.distanceToGo() == 0){
//                sensorStepperMoving = false;
//            }
//        }


        yield();
    }else if(DeviceMode == MODE_HOME){
        currentScreen.fullSend();
        delay(100);
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
  server.send(200, "text/html", "<form action=\"/MOTORFWD\" method=\"POST\"><input type=\"submit\" style=\"width:250px;height:100px;\" value=\"Move Forward\"></form><form action=\"/MOTORBKWD\" method=\"POST\"><input type=\"submit\" style=\"width:250px;height:100px;\" value=\"Move Backward\"></form><form action=\"/MOTORLEFT\" method=\"POST\"><input type=\"submit\" style=\"width:250px;height:100px;\" value=\"Move Left\"></form><form action=\"/MOTORRIGHT\" method=\"POST\"><input type=\"submit\" style=\"width:250px;height:100px;\" value=\"Move Right\"></form><form action=\"/MOTOROFF\" method=\"POST\"><input type=\"submit\" style=\"width:250px;height:100px;\" value=\"Turn Off\"></form> <form action=\"/CONVEYOR\" method=\"POST\"><input type=\"submit\" style=\"width:250px;height:100px;\" value=\"Test Conveyor\"></form><form action=\"/RACK\" method=\"POST\"><input type=\"submit\" style=\"width:250px;height:100px;\" value=\"Test Rack\"></form><form action=\"/DOWNLOAD\" method=\"POST\"><input type=\"submit\" style=\"width:250px;height:100px;\" value=\"Download Logs\"></form>");
}


void handleNotFound(){

    server.send(404, "text/plain", "404: Not found"); // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
}
void handleConveyor(){
    stepperSensor.setMaxSpeed(2000.0);
    stepperSensor.setAcceleration(100.0);
    stepperSensor.setSpeed(700);
    stepperSensor.moveTo(FULL_REVOLUTION);

    server.sendHeader("Location","/");        // Add a header to respond with a new location for the browser to go to the home page again
    server.send(303);                         // Send it back to the browser with an HTTP status 303 (See Other) to redirect
    while(stepperSensor.distanceToGo() != 0 ){
        stepperSensor.run();
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

void handleDownloadLogs(){
    server.sendHeader("Location", "/data.csv");
    server.send(303);
    delay(200);
    server.sendHeader("Location", "/");
    server.send(303);
}

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
