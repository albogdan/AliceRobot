## ALICE ROBOT - ENGINEERING DESIGN PROJECT

Second year engineering design course where we have to create a robot which stacks tires onto poles depending on the distance from a starting line and the number of tires previously on the pole.

This project consists of code on two different boards, integrated together:
1. The PIC18F4620 microcontroller by Microchip is used to handle low level user input, LCD displays, and motor control and feedback.
2. The ESP8266 WiFi chip, integrated with the PIC microcontroller through USART is used to handle higher level implementations including the robot's logic, data storage and presentation, and WiFi control.

The Alice Robot folder contains the PIC code, whilst the ESP folder contains the ESP code. As of writing this README, the PIC code is on its second iteration, streamlining it from the first one. The ESP code is functional and tested but it exists in fragments within the ESP folder, which remain to be integrated together along with the robot logic.
