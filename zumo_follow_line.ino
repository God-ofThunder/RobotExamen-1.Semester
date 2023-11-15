#include <Wire.h>
#include <Zumo32U4.h>

Zumo32U4Motors motors;
Zumo32U4LineSensors lineSensors;
Zumo32U4ButtonA buttonA;
Zumo32U4LCD lcd;

#define NUM_SENSORS 5
uint16_t lineSensorValues[NUM_SENSORS];


void readLineSensors(){
  lineSensors.read(lineSensorValues, QTR_EMITTERS_ON);
}

/*
Thresholds for sensorerne på træbanen

Linesensor 0 400
Linesensor 1 225
Linesensor 2 200
Linesensor 3 225
Linesensor 4 400
*/


void setup() {
  // put your setup code here, to run once:
  lineSensors.initFiveSensors();
  Serial.begin(9600);
  randomSeed(analogRead(0));
}

void challenge2 (){

  int speed = 200;
  int threshold_1 = 250;
  int threshold2 = 600;
  int differential1 = 250;
  int differential2 = 700;

  readLineSensors();
  if (lineSensorValues[2]>threshold_1){ //Hvis sensor 0 og 4 ikke ser linjen, korrigerer den med sensor 1, 2 og 3
    motors.setSpeeds(speed,speed);
    if (lineSensorValues[1]>threshold_1){
      motors.setSpeeds(speed - differential1 , speed);
   }
    else if (lineSensorValues[3]>threshold_1){
      motors.setSpeeds(speed , speed - differential1);
    }
  }
  if (lineSensorValues[0]>threshold2){ //Hvis sensor 0 eller 4 ser linjen drejer den skarpt
    motors.setSpeeds(speed - differential2 , speed);
    delay(50);
  }
  if (lineSensorValues[4]>threshold2){ 
    motors.setSpeeds(speed,speed-differential2);
    delay(50);
  }
}

void loop() {
challenge2();
}