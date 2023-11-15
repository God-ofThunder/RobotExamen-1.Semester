#include <Zumo32U4.h>
#include <Wire.h>

Zumo32U4LCD display;
Zumo32U4Motors motors;
Zumo32U4ButtonA buttonA;
Zumo32U4ButtonB buttonB;
Zumo32U4Encoders encoders;
Zumo32U4LineSensors lineSensors;

#define NUM_SENSORS 5
uint16_t lineSensorValues[NUM_SENSORS];
bool useEmitters = true;
uint16_t surfaceBrightnes[NUM_SENSORS];
bool lineSensorsToExclude[NUM_SENSORS] = {false, true, true, true, false};
uint8_t lineTreshHold = 350;

// checks if there is a line and returns the sensor number that detected the line
// checks from sensor 0 to 4 and returns only the only the first sensor that detected the line 
// so exklude the sensor that should not be checkt
// returns -1 if no line is detected
int8_t DetectLine(){
	// read line sensor values 
	lineSensors.read(lineSensorValues, useEmitters ? QTR_EMITTERS_ON: QTR_EMITTERS_OFF);
	// check if a sensor detect a line
	for(int i=0; i<NUM_SENSORS; i++){
		if(lineSensorsToExclude[i] != true){  
			if(lineSensorValues[i] < surfaceBrightnes[i]-lineTreshHold || lineSensorValues[i] > surfaceBrightnes[i]+lineTreshHold)
				return i;
		}
	}
	// returns -1 if not detected a line
	return -1;
}

// drives until line detected
void DriveToLine(){
		while(DetectLine() == -1){
			delay(1);
		}
		motors.setSpeeds(0,0);
}

// drive until line detected or time is out
void DriveForTimeOrToLine(uint16_t time){
	uint16_t cykles = time/10;
	for(int i=0; i<cykles; i++){
		if(DetectLine() != -1)
			break;
		delay(10);
	}
	motors.setSpeeds(0,0);
}

// checks if lined up
bool CheckIfLinedUP(){
	// drives to the line 
	motors.setSpeeds(80,80);
	DriveToLine();

	// detect a line and stors the sensor number 
	int8_t sensor1 = DetectLine();
	// if the number is -1 then no line is detected so its not lined up
	if(sensor1 == -1)
		return false;	

	// sets to exklude the sensor that detected the line 
	lineSensorsToExclude[sensor1] = true;
	
	display.clear();
	display.gotoXY(0,0);
	display.print(sensor1);
	display.gotoXY(0,1);
	display.print(DetectLine());
	
	// checks if a sensor detected a line and if so the robot is lined up so return true 
	if(DetectLine() != -1){
		lineSensorsToExclude[sensor1] = false;
		return true;
	}

	// if no other sensor detected a line the robot is not lined up so return false 
	lineSensorsToExclude[sensor1] = false;
	motors.setSpeeds(0,0);
	return false;


}

// lines up
void LineUP(){
	bool linedUP = false;
	while(linedUP != true){
		motors.setSpeeds(80,80);
		DriveToLine();
		// DetectLine returns the sensor number which detected the line
		if(DetectLine() == 0){
			lineSensorsToExclude[0] = true;
			motors.setSpeeds(0,80);
			DriveToLine();
			lineSensorsToExclude[0] = false;
		}else if(DetectLine() == 4){
			lineSensorsToExclude[4] = true;
			motors.setSpeeds(80,0);
			DriveToLine();
			lineSensorsToExclude[4] = false;
		}
		else if(DetectLine() == 2){
			lineSensorsToExclude[2] = true;
			uint16_t time = 50;
			while(DetectLine() == -1){
				// check for line left
				motors.setSpeeds(80,0);
				DriveForTimeOrToLine(time);
				// check for line right
				motors.setSpeeds(0,80);
				DriveForTimeOrToLine(time*2);
				// drive back
				motors.setSpeeds(80,0);
				DriveForTimeOrToLine(time);
				time += 10;
			}
			lineSensorsToExclude[0] = false;
		}
		motors.setSpeeds(-80,-80);
		delay(100);
		motors.setSpeeds(0,0);
		linedUP = CheckIfLinedUP();
	}
}

void setup() {
	// initialises the encoders and resets the counts 
	encoders.init();
	encoders.getCountsAndResetLeft();
	encoders.getCountsAndResetRight();

	lineSensors.initFiveSensors();
	
	// displays "push A to calibrate" and waits for button A to be pressed
	display.clear();
	display.gotoXY(0,0);
	display.print("push A to");
	display.gotoXY(0,1);
	display.print("calibrate");
	buttonA.waitForPress();
	
	// sets base valuses for the line sensors 
	lineSensors.read(lineSensorValues, useEmitters ? QTR_EMITTERS_ON: QTR_EMITTERS_OFF);
	for(int i=0; i<NUM_SENSORS; i++){
		surfaceBrightnes[i] = lineSensorValues[i];
	}

	// displays "push B to start" and wauts for button B to be pressed
	display.clear();
	display.gotoXY(0,0);
	display.print("push B to");
	display.gotoXY(0,1);
	display.print("start");
	buttonB.waitForPress();

	LineUP();
}
bool finished = false;
void loop() {
	while(finished != true){
		lineSensors.read(lineSensorValues, useEmitters ? QTR_EMITTERS_ON: QTR_EMITTERS_OFF);
		for(int i=0; i<NUM_SENSORS; i+=2)
			if(lineSensorValues[i] < surfaceBrightnes[i]-lineTreshHold || lineSensorValues[i] > surfaceBrightnes[i]+lineTreshHold)
				finished = true;
		if(encoders.getCountsLeft() < encoders.getCountsRight())
			motors.setSpeeds(120,40);
		else if(encoders.getCountsLeft() > encoders.getCountsRight())
			motors.setSpeeds(40,120);
		else
			motors.setSpeeds(80,80);
		delay(100);
		//encoders.getCountsAndResetRight();
		//encoders.getCountsAndResetLeft();
	}
	motors.setSpeeds(0,0);
	display.clear();
	display.gotoXY(0,0);
	display.print("finished");
	delay(1000);
}
