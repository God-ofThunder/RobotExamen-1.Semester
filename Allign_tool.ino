
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
Zumo32U4Encoders encoders;
Zumo32U4IMU imu;

int movementCommand = 0;   // used to select the command
int movementParameter = 0;   // used to select distance or angle
float wheelCirc = 13;

uint32_t turnAngle = 0;
int16_t turnRate;
int16_t gyroOffset;
uint16_t gyroLastUpdate = 0;
float Scalar = 1.01;


int countsL = encoders.getCountsLeft();
int countsR = encoders.getCountsRight();

float distanceL = countsL/900.0 * wheelCirc;
float distanceR = countsR/900.0 * wheelCirc;


#define NUM_SENSORS 4
uint16_t lineSensorValues[NUM_SENSORS];

char buffer[80];

int ThreshOff = 600;
int ThreshMid = 500;
int ThreshSides = 800;

void TurnSensorSetup()
{
  Wire.begin();
  imu.init();
  imu.enableDefault();
  imu.configureForTurnSensing();

  display.clear();
  display.print(F("Gyro cal"));

  // Delay to give the user time to remove their finger.
  delay(500);

  // Calibrate the gyro.
  int32_t total = 0;
  for (uint16_t i = 0; i < 1024; i++)
  {
    // Wait for new data to be available, then read it.
    while(!imu.gyroDataReady()) {}
    imu.readGyro();

    // Add the Z axis reading to the total.
    total += imu.g.z;
  }
  gyroOffset = total / 1024;

  // Display the angle (in degrees from -180 to 180) until the
  // user presses A.
  display.clear();
  turnSensorReset();
}

void turnSensorReset()
{
  gyroLastUpdate = micros();
  turnAngle = 0;
}

void turnSensorUpdate()
{
  // Read the measurements from the gyro.
  imu.readGyro();
  turnRate = imu.g.z - gyroOffset;

  // Figure out how much time has passed since the last update (dt)
  uint16_t m = micros();
  uint16_t dt = m - gyroLastUpdate;
  gyroLastUpdate = m;

  // Multiply dt by turnRate in order to get an estimation of how
  // much the robot has turned since the last update.
  // (angular change = angular velocity * time)
  int32_t d = (int32_t)turnRate * dt;

  // The units of d are gyro digits times microseconds.  We need
  // to convert those to the units of turnAngle, where 2^29 units
  // represents 45 degrees.  The conversion from gyro digits to
  // degrees per second (dps) is determined by the sensitivity of
  // the gyro: 0.07 degrees per second per digit.
  //
  // (0.07 dps/digit) * (1/1000000 s/us) * (2^29/45 unit/degree)
  // = 14680064/17578125 unit/(digit*us)
  turnAngle += (int64_t)d * 14680064 / 17578125;
}

uint32_t getTurnAngleInDegrees(){
  turnSensorUpdate();
  // do some math and pointer magic to turn angle in seconds to angle in degree
  return (((uint32_t)turnAngle >> 16) * 360) >> 16;
}

float getDistanceR(){
  int countsL = encoders.getCountsLeft();
  int countsR = encoders.getCountsRight();

  float distanceL = countsL/900.0 * wheelCirc;
  float distanceR = countsR/900.0 * wheelCirc;

  return(distanceR);
}

float getDistanceL(){
  int countsL = encoders.getCountsLeft();
  int countsR = encoders.getCountsRight();

  float distanceL = countsL/900.0 * wheelCirc;
  float distanceR = countsR/900.0 * wheelCirc;

  return(distanceL);
}

void resetEncoders(){
  encoders.getCountsAndResetLeft();
  encoders.getCountsAndResetRight();
}

void forward(){
  motor.setSpeeds(100,100);
}

void backward(){
  motor.setSpeeds(-100,-100);
}

void adjustLeft(){
  motor.setSpeeds(10,100);
  delay(5);
}

void adjustRight(){
  motor.setSpeeds(100,10);
  delay(5);
}

void Stop(){
  motor.setSpeeds(0,0);
  delay(1000000);
}

void SendIt(){
  for (int i=0;i<5;){
    display.clear();
    motor.setSpeeds(200,200);
    Serial.print(getTurnAngleInDegrees());
    float vinkel = getTurnAngleInDegrees();
    display.print(getDistanceR());
    display.gotoXY(0,1);
    display.print(getDistanceL());
    while (getDistanceL() < getDistanceR() * Scalar){
      motor.setSpeeds(200,180);
  }
    while (getDistanceR() * Scalar < getDistanceL()){
      motor.setSpeeds(180,200);
  }
  lineSensors.read(lineSensorValues, QTR_EMITTERS_ON);
  if (lineSensorValues[0] > ThreshSides || lineSensorValues[1] > ThreshOff || lineSensorValues[2] > ThreshMid || lineSensorValues[3] > ThreshOff || lineSensorValues[4] > ThreshSides){
    i=5;
  }
    
}
}

void Woopsies(){
  for (int i=0;i<5;){
  lineSensors.read(lineSensorValues, QTR_EMITTERS_ON);
  if (lineSensorValues[0] > ThreshSides){
    backward();
    delay(500);
    motor.setSpeeds(-100,100);
    delay(750);
    forward();
    delay(300);
    motor.setSpeeds(100,-100);
    delay(750);
    i=5;
  }
  
  else if (lineSensorValues[4] > ThreshSides){
    backward();
    delay(500);
    motor.setSpeeds(100,-100);
    delay(750);
    forward();
    delay(300);
    motor.setSpeeds(-100,100);
    delay(750);
    i=5;
  }
}
}

void Adjusting(){
  for(int i = 0; i < 6;){
  lineSensors.read(lineSensorValues, QTR_EMITTERS_ON);
  forward();
   if (lineSensorValues[0] > ThreshSides){
    i++;
    while (lineSensorValues[4] < ThreshSides){
      lineSensors.read(lineSensorValues, QTR_EMITTERS_ON);
      adjustLeft();
    }
    backward();
    delay(300);
  }

  else if (lineSensorValues[4] > ThreshSides){
    i++;
    while (lineSensorValues[0] < ThreshSides){
      lineSensors.read(lineSensorValues, QTR_EMITTERS_ON);
      adjustRight();
    }
    backward();
    delay(300);
  }
   else if (lineSensorValues[2] > ThreshMid){
    Woopsies();
    }
  }
}
void AdjustLite(){
  for(int i=0;i<5;){
    lineSensors.read(lineSensorValues, QTR_EMITTERS_ON);
    forward();
    if(lineSensorValues[4] > ThreshSides && lineSensorValues[0] > ThreshSides){
      i=5;
  }
}
}

void setup() {
  // put your setup code here, to run once:
  lineSensors.initFiveSensors();
  TurnSensorSetup();
  Serial.begin(9600);
  while(buttonA.isPressed() != 1){
  lineSensors.read(lineSensorValues, QTR_EMITTERS_ON);
  sprintf(buffer, "%4d %4d %4d %4d %4d\n",
  lineSensorValues[0],
  lineSensorValues[1],
  lineSensorValues[2],
  lineSensorValues[3],
  lineSensorValues[4]);
  Serial.print(buffer);
  }
}


void loop(){
  Adjusting();
  AdjustLite();
  SendIt();
  Stop();

}
