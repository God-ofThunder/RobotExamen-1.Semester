
#include <Wire.h>
#include <Zumo32U4.h>
#include <Zumo32U4IRPulses.h>
// Change next line to this if you are using the older Zumo 32U4
// with a black and green LCD display:
// Zumo32U4LCD display;
Zumo32U4OLED display;
Zumo32U4ProximitySensors proxSensor;
Zumo32U4LineSensors lineSensors;
Zumo32U4ButtonA buttonA;
Zumo32U4Motors motor;

char buffer[80];
uint16_t lineSensorValues[3] = { 0, 0, 0};

bool proxLeftActive;
bool proxFrontActive;
bool proxRightActive;
int stage = 0;
float Thresh = 6000;
int i = 0;

float ProxHojre = 0;
float ProxVenstre = 0;

void setup() {
    lineSensors.initThreeSensors();
    Serial.begin(9600);
    proxSensor.initThreeSensors();
    proxSensor.setBrightnessLevels(25,200);//første er level, næste er count brightness
    proxSensor.setPeriod(200);
    while(buttonA.isPressed() != 1){//når bA ikke er trykket ned, så kører den i wloop.
    proxSensor.read();
    lineSensors.read(lineSensorValues, QTR_EMITTERS_ON);
    sprintf(buffer, "%4d %4d %4d\n",
    lineSensorValues[0],
    lineSensorValues[1],
    lineSensorValues[2]);
    Serial.print(buffer);
    int proxRight = proxSensor.countsRightWithRightLeds();
    int proxLeft = proxSensor.countsLeftWithLeftLeds();
    display.clear();
    display.gotoXY(0,0); //levels aflæses før press A, når det er sat i whileloop i setup
    display.print(proxSensor.countsRightWithRightLeds ());
    display.gotoXY(0,1); 
    display.print(proxSensor.countsLeftWithLeftLeds());
    Serial.print(proxSensor.countsLeftWithLeftLeds());
    Serial.print(proxSensor.countsLeftWithRightLeds());

  }
  //buttonA.getSingleDebouncedPress(); //den er god at have med, fordi loopet starter først når der er trykket 
  motor.setSpeeds(210,200);
  // put your setup code here, to run once:

}

void loop() {
  switch(stage){
    case 0: TurnMore();
    break;
    case 1: Stop();
    break;
  }

}
int Divider = 8;
void Turn(){
      proxSensor.read();
      //int proxSensor.countsRightWithRightLeds () = proxSensor.countsRightWithRightLeds ();
      //int proxSensor.countsLeftWithLeftLeds() = proxSensor.countsLeftWithLeftLeds();
      display.clear();
      display.gotoXY(0,0);
      display.print(proxSensor.countsRightWithRightLeds ());
      display.gotoXY(0,1); 
      display.print(proxSensor.countsLeftWithLeftLeds());
      motor.setSpeeds(110,100);
      ProxHojre = 0;
      ProxVenstre = 0;
      for (int i = 0; i < Divider; i++){
        proxSensor.read();
        ProxHojre = proxSensor.countsRightWithRightLeds() + ProxHojre;
        ProxVenstre = proxSensor.countsLeftWithLeftLeds() + ProxVenstre;
      }
      ProxHojre = ProxHojre / Divider;
      ProxVenstre = ProxVenstre / Divider;
      if (ProxHojre < ProxVenstre){
          motor.setSpeeds(110,-50);
  }

      else if (ProxVenstre < ProxHojre){
          motor.setSpeeds(-60,100);
  }
  
}

void TurnMore() {
  lineSensors.read(lineSensorValues, QTR_EMITTERS_ON);
  if (lineSensorValues[0]> Thresh){
    stage = 1;
  }
  
  else if (lineSensorValues[1]> Thresh){
    stage = 1;
  }
  else if (lineSensorValues[2]> Thresh){
    stage = 1;
  }
  else{
    Turn();
  }
    
}

void Stop(){
  motor.setSpeeds(0,0);
  while(buttonA.isPressed() != 1){
    
  }
  delay(2000);
  stage = 0;
}

/*void proxSensorUpdate(){
  proxSensor.read();
  int proxSensor.countsRightWithRightLeds () = proxSensor.countsRightWithRightLeds ();
  int proxSensor.countsLeftWithLeftLeds() = proxSensor.countsLeftWithLeftLeds();
}*/
