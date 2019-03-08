#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266mDNS.h> 
#include <ESP8266WebServer.h>

ESP8266WiFiMulti wifiMulti;     // Create an instance of the ESP8266WiFiMulti class, called 'wifiMulti'
ESP8266WebServer server(80); // Create a webserver object that listens for HTTP request on port 80

void handleRoot();
void handleMotorFWD();
void handleMotorBKWD();
void handleMotorOFF();
void handleNotFound();


void setup() {
  Serial.begin(19200);
  delay(10);

  wifiMulti.addAP("JUPITER", "epvg8092");
  
  while (wifiMulti.run() != WL_CONNECTED) { // Wait for the Wi-Fi to connect: scan for Wi-Fi networks, and connect to the strongest of the networks above
    delay(250);
    Serial.print('.');
  }

  Serial.println('\n');
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());              // Tell us what network we're connected to
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());           // Send the IP address of the ESP8266 to PC

  if (MDNS.begin("alicebot")) {              // Start the mDNS responder for esp8266.local
    Serial.println("mDNS responder started");
  } else {
    Serial.println("Error setting up MDNS responder!");
  }

  server.on("/", HTTP_GET, handleRoot);     // Call the 'handleRoot' function when a client requests URI "/"
  server.on("/MOTORFWD", HTTP_POST, handleMotorFWD);  // Call the 'handleLED' function when a POST request is made to URI "/LED"
  server.on("/MOTORBKWD", HTTP_POST, handleMotorBKWD);  // Call the 'handleLED' function when a POST request is made to URI "/LED"
  server.on("/MOTOROFF", HTTP_POST, handleMotorOFF);  // Call the 'handleLED' function when a POST request is made to URI "/LED"
  server.onNotFound(handleNotFound);        // When a client requests an unknown URI (i.e. something other than "/"), call function "handleNotFound"

  server.begin();
}

void loop() {
  server.handleClient();
}

void handleRoot(){
  server.send(200, "text/html", "<form action=\"/MOTORFWD\" method=\"POST\"><input type=\"submit\" style=\"width:250px;height:100px;\" value=\"Move Forward\"></form><form action=\"/MOTORBKWD\" method=\"POST\"><input type=\"submit\" style=\"width:250px;height:100px;\" value=\"Move Backward\"></form><form action=\"/MOTOROFF\" method=\"POST\"><input type=\"submit\" style=\"width:250px;height:100px;\" value=\"Turn Off\"></form>");
}


void handleNotFound(){

  server.send(404, "text/plain", "404: Not found"); // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
}

void handleMotorFWD(){
  Serial.write("MFN\n");
  server.sendHeader("Location","/");        // Add a header to respond with a new location for the browser to go to the home page again
  server.send(303);                         // Send it back to the browser with an HTTP status 303 (See Other) to redirect
}
void handleMotorBKWD(){
  Serial.write("MBN\n");
  server.sendHeader("Location","/");        // Add a header to respond with a new location for the browser to go to the home page again
  server.send(303);                         // Send it back to the browser with an HTTP status 303 (See Other) to redirect  
}
void handleMotorOFF(){
  Serial.write("MFF\n");
  server.sendHeader("Location","/");        // Add a header to respond with a new location for the browser to go to the home page again
  server.send(303);                         // Send it back to the browser with an HTTP status 303 (See Other) to redirect
}
