#include <Wire.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WebServer.h>

#include "Adafruit_VL53L0X.h"

Adafruit_VL53L0X lox = Adafruit_VL53L0X();

void handleRoot();
void handleMotorFWD();
void handleMotorBKWD();
void handleMotorOFF();
void handleNotFound();

ESP8266WiFiMulti wifiMulti;     // Create an instance of the ESP8266WiFiMulti class, called 'wifiMulti'
ESP8266WebServer server(80); // Create a webserver object that listens for HTTP request on port 80



void setup() {
    Serial.begin(19200);
    delay(10);
    Serial.println("Serial started");
    wifiMulti.addAP("JUPITER", "epvg8092");    
    while (wifiMulti.run() != WL_CONNECTED) { // Wait for the Wi-Fi to connect: scan for Wi-Fi networks, and connect to the strongest of the networks above
      delay(250);
      Serial.print('.');
    }
    Serial.println(WiFi.localIP());           // Send the IP address of the ESP8266 to PC


    if (!lox.begin()) {
        Serial.println(F("Failed to boot VL53L0X"));
        while(1);
    }
    server.on("/", HTTP_GET, handleRoot);     // Call the 'handleRoot' function when a client requests URI "/"
    server.on("/MOTORRIGHT", HTTP_POST, handleMotorRight);
    server.on("/MOTORLEFT", HTTP_POST, handleMotorLeft);
    server.on("/MOTORFWD", HTTP_POST, handleMotorFWD);  // Call the 'handleLED' function when a POST request is made to URI "/LED"
    server.on("/MOTORBKWD", HTTP_POST, handleMotorBKWD);  // Call the 'handleLED' function when a POST request is made to URI "/LED"
    server.on("/MOTOROFF", HTTP_POST, handleMotorOFF);  // Call the 'handleLED' function when a POST request is made to URI "/LED"

//    server.on("/RACK", HTTP_POST, handleRack);
    server.onNotFound(handleNotFound);        // When a client requests an unknown URI (i.e. something other than "/"), call function "handleNotFound"
    server.begin();

}
bool moving = false;
void loop() {
    VL53L0X_RangingMeasurementData_t measureStart;
    lox.rangingTest(&measureStart,false); //pass in 'true' to get debug data printout
     //Check laser sensor if detects object (distance between 10 and 30 cm?)
    if(moving && measureStart.RangeStatus!=4 && (measureStart.RangeMilliMeter > 100 && measureStart.RangeMilliMeter < 300)){
        moving = false;
        Serial.print("D1Pole detected\n");
        Serial.write("MFF\n");
    }
    server.handleClient();
    yield();
}



void handleRoot(){
    server.send(200, "text/html", "<form action=\"/MOTORFWD\" method=\"POST\"><input type=\"submit\" style=\"width:250px;height:100px;\" value=\"Move Forward\"></form><form action=\"/MOTORBKWD\" method=\"POST\"><input type=\"submit\" style=\"width:250px;height:100px;\" value=\"Move Backward\"></form><form action=\"/MOTORLEFT\" method=\"POST\"><input type=\"submit\" style=\"width:250px;height:100px;\" value=\"Move Left\"></form><form action=\"/MOTORRIGHT\" method=\"POST\"><input type=\"submit\" style=\"width:250px;height:100px;\" value=\"Move Right\"></form><form action=\"/MOTOROFF\" method=\"POST\"><input type=\"submit\" style=\"width:250px;height:100px;\" value=\"Turn Off\"></form> <form action=\"/CONVEYOR\" method=\"POST\"><input type=\"submit\" style=\"width:250px;height:100px;\" value=\"Test Conveyor\"></form><form action=\"/RACK\" method=\"POST\"><input type=\"submit\" style=\"width:250px;height:100px;\" value=\"Test Rack\"></form>");
}
void handleNotFound(){
    server.send(404, "text/plain", "404: Not found"); // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
}
void handleMotorFWD(){
    Serial.write("MFB\n");
    moving = true;
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
    moving = true;
    server.sendHeader("Location","/");        // Add a header to respond with a new location for the browser to go to the home page again
    server.send(303);                         // Send it back to the browser with an HTTP status 303 (See Other) to redirect  
}
void handleMotorOFF(){
    Serial.write("MFF\n");
    server.sendHeader("Location","/");        // Add a header to respond with a new location for the browser to go to the home page again
    server.send(303);                         // Send it back to the browser with an HTTP status 303 (See Other) to redirect
}
