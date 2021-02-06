/*
   Copyright (c) 2015, Majenko Technologies
   All rights reserved.

   Redistribution and use in source and binary forms, with or without modification,
   are permitted provided that the following conditions are met:

 * * Redistributions of source code must retain the above copyright notice, this
     list of conditions and the following disclaimer.

 * * Redistributions in binary form must reproduce the above copyright notice, this
     list of conditions and the following disclaimer in the documentation and/or
     other materials provided with the distribution.

 * * Neither the name of Majenko Technologies nor the names of its
     contributors may be used to endorse or promote products derived from
     this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
   ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
   ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/* Create a WiFi access point and provide a web server on it. */


/* 
This programm takes a string from a serial port, parses it
and move servo on a given angle.
Each message must be like (angle in degrees):
  "[X]:[Y]:[H]:[SERVO1_ANGLE]:[SERVO2_ANGLE]:[SERVO3_ANGLE]:[SERVO4_ANGLE]$"
*/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <Servo.h>


const int SERVO1_MIN_ANGLE = 52;
const int SERVO2_MIN_ANGLE = 55;
const int SERVO3_MIN_ANGLE = 46;
const int SERVO4_MIN_ANGLE = 55;



const int SERVO1_MAX_ANGLE = 152;
const int SERVO2_MAX_ANGLE = 155;
const int SERVO3_MAX_ANGLE = 146;
const int SERVO4_MAX_ANGLE = 155;


const int SERVO_MAP_MAX_ANGLE =  50;   // In percents
const int SERVO_MAP_MIN_ANGLE = -50;   //

const int SERVO1_PIN = D2;//2
const int SERVO2_PIN = D5;//5
const int SERVO3_PIN = D6;//6
const int SERVO4_PIN = D7;//7
//:::::::::::::::::::::::::::::::\\


#ifndef APSSID
#define APSSID "BALLBOUNCER"
#define APPSK  "12345678"
#endif

/* Set these to your desired credentials. */
const char *ssid = APSSID;
const char *password = APPSK;

ESP8266WebServer server(80);


Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;

String inData;  // Buffer for getting data
double servo1_angle = 0;
double servo2_angle = 0;
double servo3_angle = 0;
double servo4_angle = 0;

int x=0;
int y=0;
int h=0;

/* Just a little test message.  Go to http://192.168.4.1 in a web browser
   connected to this access point to see it.
*/
void handleRoot() {
  

  String message = "{";
  message += " \"X\":";
  message += x;
  message += ", \"Y\":";
  message += y;
  message += ", \"H\":";
  message += h;
  message += ", \"X1\":";
  message += servo1_angle;
  message += ", \"X2\":";
  message += servo2_angle;
  message += ", \"Y1\":";
  message += servo3_angle;
  message += ", \"Y2\":";
  message += servo4_angle;
  message += " }";
  server.send(200, "application/json", message);
}


void setup() {
  delay(1000);
  Serial.begin(115200);
  
  servo1.attach(SERVO1_PIN);
  servo2.attach(SERVO2_PIN);
  servo3.attach(SERVO3_PIN);
  servo4.attach(SERVO4_PIN);

 
  servo1.write(102);  
  servo2.write(105);  
  servo3.write(96);  
  servo4.write(105); 
  
  Serial.println();
  Serial.print("Configuring access point...");
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.on("/", handleRoot);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  if (Serial.available() > 0) {
  
    // Reading angle of the servo1
    inData = Serial.readStringUntil(':');
    servo1_angle = inData.toInt();

    // Reading angle of the servo2
    inData = Serial.readStringUntil(':');
    servo2_angle = inData.toInt();

    // Reading angle of the servo3
    inData = Serial.readStringUntil(':');
    servo3_angle = inData.toInt();
    
    // Reading angle of the servo4
    inData = Serial.readStringUntil(':');
    servo4_angle = inData.toInt();

    // Reading angle of the x
    inData = Serial.readStringUntil(':');
    x = inData.toInt();

    // Reading angle of the y
    inData = Serial.readStringUntil(':');
    y = inData.toInt();

    // Reading angle of the h
    inData = Serial.readStringUntil('$');
    h = inData.toInt();

    // Checking if the angles are correct
    if (servo1_angle < SERVO_MAP_MIN_ANGLE || servo1_angle > SERVO_MAP_MAX_ANGLE ||
        servo2_angle < SERVO_MAP_MIN_ANGLE || servo2_angle > SERVO_MAP_MAX_ANGLE ||
        servo3_angle < SERVO_MAP_MIN_ANGLE || servo3_angle > SERVO_MAP_MAX_ANGLE ||
        servo4_angle < SERVO_MAP_MIN_ANGLE || servo4_angle > SERVO_MAP_MAX_ANGLE) {
      
      //Serial.println("Angles are not correct. Not in the range of allowed values.");
      
    } else {
      
      // Mapping real angles to servs' range of angles
      servo1_angle = map(servo1_angle,
                         SERVO_MAP_MAX_ANGLE, SERVO_MAP_MIN_ANGLE,
                         SERVO1_MAX_ANGLE, SERVO1_MIN_ANGLE);
                         
      servo2_angle = map(servo2_angle,
                         SERVO_MAP_MAX_ANGLE, SERVO_MAP_MIN_ANGLE,
                         SERVO2_MAX_ANGLE, SERVO2_MIN_ANGLE);  

      servo3_angle = map(servo3_angle,
                         SERVO_MAP_MAX_ANGLE, SERVO_MAP_MIN_ANGLE,
                         SERVO3_MAX_ANGLE, SERVO3_MIN_ANGLE);
                                      
      servo4_angle = map(servo4_angle,
                         SERVO_MAP_MAX_ANGLE, SERVO_MAP_MIN_ANGLE,
                         SERVO4_MAX_ANGLE, SERVO4_MIN_ANGLE);
                                            
      // Writing mapped angles to servos
      servo1.write(servo1_angle);
      servo2.write(servo2_angle);
      servo3.write(servo3_angle);
      servo4.write(servo4_angle);
    }
  }
  server.handleClient();
}
