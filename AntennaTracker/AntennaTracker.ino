// Sweep
// by BARRAGAN <http://barraganstudio.com>
// This example code is in the public domain.


#include <Servo.h>

Servo pan;  // pan servo
Servo tilt; // tilt servo

// Pin 13 has an LED connected on most Arduino boards.
int led = 13;
long lastTilt = 65L;
long lastPan = 50L;
int dir = 1;
int pos = 0;
int tiltDelay = 15;
long minPWM = 1000L;
long maxPWM = 2000L;
long panVal = 0L;
long tiltVal = 0L;
long interpVal = 0L;

String cmd = "";
String panStr = "";
String tiltStr = "";

void setup(){
	pan.attach(9);
	tilt.attach(10);

	Serial.begin(57600);

	pinMode(led, OUTPUT);

	// Origin for both servos
	tilt.write(lastTilt);
	pan.write(lastPan);
}


void loop(){
	if (Serial.available()){
		char ch = Serial.read();

		if (ch == '\r'){
			digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)

			// Line feed is the command char
			if (cmd.startsWith("!!!PAN:")){
				panStr = cmd.substring(7,11);

				if (cmd.substring(12,16) == "TLT:"){
					tiltStr = cmd.substring(16,20);
				}

				if (panStr != ""){
					panVal = panStr.toInt();
					if (panVal < minPWM){
						panVal = minPWM;
					}

					if (panVal > maxPWM){
						panVal = maxPWM;
					}

					interpVal = ((panVal-minPWM) * 360) / (maxPWM-minPWM);

					if (interpVal > 360){
						interpVal = 360;
					}

					if (interpVal < 0){
						interpVal = 0;
					}

					Serial.print("Pan=");
					Serial.println(interpVal);

					// This is the calibration constant for the pan which has a gear ratio
					interpVal = ((interpVal * 10) / 37) + 50;
					pan.write(interpVal);
					lastPan = interpVal;
				}

				if (tiltStr != ""){
					tiltVal = tiltStr.toInt();
					if (tiltVal < minPWM){
						tiltVal = minPWM;
					}

					if (tiltVal > maxPWM){
						tiltVal = maxPWM;
					}

					interpVal = ((tiltVal-minPWM) * 90) / (maxPWM-minPWM);

					if (interpVal > 90){
						interpVal = 90;
					}

					if (interpVal < 0){
						interpVal = 0;
					}

					Serial.print("Tilt=");
					Serial.println(interpVal);

					// This is the calibration constant
					interpVal = 150 - interpVal;
	
					dir = 1;
					if (interpVal < lastTilt){
						dir = -1;
					}

					if (tiltDelay > 0){
						// Slow down tilt by stepping to it's destination
						for (pos = lastTilt; pos != interpVal; pos += dir){
							tilt.write(pos);
							delay(tiltDelay);
						}
					}else{
						tilt.write(interpVal);
					}

					lastTilt = interpVal;
				}
			}else if (cmd.startsWith("!!!TLTDLY:")){
				cmd = cmd.substring(10,14);
				tiltDelay = cmd.toInt();
			}

			panStr = "";
			tiltStr == "";
			cmd = "";

			digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
		}else{
			cmd += ch;
		}
	}
}
