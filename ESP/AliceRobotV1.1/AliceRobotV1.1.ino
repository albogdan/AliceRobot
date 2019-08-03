#include <RTClib.h>

#include "Screen.h"
#include <Wire.h>
#include <FS.h>   // Include the SPIFFS library
//#include <ESP8266WiFi.h>
//#include <WiFiClient.h>
#include <ESP8266WiFiMulti.h>
//#include <ESP8266mDNS.h> 
#include <ESP8266WebServer.h>

#define MODE_HOME 1
#define MODE_RUNNING 2
#define MODE_LOGS 3
#define MODE_SPECIFIC_LOG 4

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
Screen runningScreen("Running...\n", "", "NOW", "\n",0, rtc);
Screen emptyScreen("                \n","                \n","                \n","                \n",0,rtc);
Screen logScreen;
Screen specificLog;
Screen currentScreen;

int DeviceMode;
DateTime startTime;
DateTime endTime;
bool firstStart;
String characters = "0123456789";

void setup() {
    DeviceMode = 1;
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
    server.onNotFound(handleNotFound);        // When a client requests an unknown URI (i.e. something other than "/"), call function "handleNotFound"

  server.begin();

      
    currentScreen = homeScreen;
    emptyScreen.fullSend();

}

void loop() {
  String serialValue;
  server.handleClient();
  if(stringComplete){
    if(DeviceMode == MODE_HOME){
        if(inputString == "K1\n" && firstStart){
            DeviceMode = MODE_RUNNING;
            firstStart = false;
            startTime = rtc.now();
            char temp[16];
            sprintf(temp, "START: %02d:%02d:%02d\n", startTime.hour(), startTime.minute(), startTime.second());//
            String text(temp);
            runningScreen.setLine(1, text);
            emptyScreen.fullSend();
//            Serial.println("CHANGING TO RUN MODE");
            currentScreen = runningScreen;
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
  if(DeviceMode == MODE_HOME){
      currentScreen.fullSend();
  }
//  delay(500);




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
  yield();
}



/*
 * ------------WIFI FUNCTIONS START----------------------
 */
void handleRoot(){
  server.send(200, "text/html", "<form action=\"/MOTORFWD\" method=\"POST\"><input type=\"submit\" style=\"width:250px;height:100px;\" value=\"Move Forward\"></form><form action=\"/MOTORBKWD\" method=\"POST\"><input type=\"submit\" style=\"width:250px;height:100px;\" value=\"Move Backward\"></form><form action=\"/MOTORLEFT\" method=\"POST\"><input type=\"submit\" style=\"width:250px;height:100px;\" value=\"Move Left\"></form><form action=\"/MOTORRIGHT\" method=\"POST\"><input type=\"submit\" style=\"width:250px;height:100px;\" value=\"Move Right\"></form><form action=\"/MOTOROFF\" method=\"POST\"><input type=\"submit\" style=\"width:250px;height:100px;\" value=\"Turn Off\"></form>");
}


void handleNotFound(){

  server.send(404, "text/plain", "404: Not found"); // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
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
