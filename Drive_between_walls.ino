//CHALLENGE 3: The Zumo shall drive in between the walls, with equal distance to the walls at each side, untill detection of the black line at the far end of the challenge.

#include <Zumo32U4.h>
#include <Wire.h>
#include <Zumo32U4IRPulses.h>

Zumo32U4OLED display;
Zumo32U4ProximitySensors proxSensor;
Zumo32U4LineSensors lineSensors;
Zumo32U4ButtonA buttonA;
Zumo32U4Motors motor;

uint16_t lineSensorValues[3] = {0, 0, 0};

bool proxLeftActive, proxFrontActive, proxRightActive;
int stage = 0; // Stadiet, der styrer robottens opførsel
float thresh = 350; // Tærskelværdien 
int Divider = 8;

float ProxHojre = 0;
float ProxVenstre = 0;

char buffer[80];

void setup() {
  // Initialiserer sensorer og kommunikation
  lineSensors.initThreeSensors();

  proxSensor.initThreeSensors();
  proxSensor.setBrightnessLevels(25, 200);
  proxSensor.setPeriod(200);

  Serial.begin(9600);

  // Venter på der bliver trykket på knap A, loopet fortsætter så længe knap A ikke er trykket
  while (buttonA.isPressed() != 1) {
    proxSensor.read();
    lineSensors.read(lineSensorValues, QTR_EMITTERS_ON);

    // Udskriver linesensorværdier til serial monitor
    sprintf(buffer, "%4d %4d %4d\n",
            lineSensorValues[0],
            lineSensorValues[1],
            lineSensorValues[2]);
    Serial.print(buffer);

    // Viser proximitetssensorværdier på displayet
    int proxRight = proxSensor.countsRightWithRightLeds();
    int proxLeft = proxSensor.countsLeftWithLeftLeds();
    display.clear();
    display.gotoXY(0, 0); // Niveauer aflæses før tryk på A-knap
    display.print(proxSensor.countsRightWithRightLeds());
    display.gotoXY(0, 1);
    display.print(proxSensor.countsLeftWithLeftLeds());
  }

  //zumoen skal nu køre efter knap tryk
  motor.setSpeeds(100, 100);
}

void loop() { 

  switch (stage) {
    case 0:
      CheckIfLineDetected();
      Turn();
      break;
    case 1: Stop();
      break;
  }
}

void Turn() {
  proxSensor.read();
  display.clear();
  display.gotoXY(0, 0);
  display.print(proxSensor.countsRightWithRightLeds());
  display.gotoXY(0, 1);
  display.print(proxSensor.countsLeftWithLeftLeds());
  motor.setSpeeds(100, 100);
  

  for (int i = 0; i < Divider; i++) {
    proxSensor.read();
    ProxHojre = proxSensor.countsRightWithRightLeds() + ProxHojre;
    ProxVenstre = proxSensor.countsLeftWithLeftLeds() + ProxVenstre;
  }

  ProxHojre = ProxHojre / Divider;
  ProxVenstre = ProxVenstre / Divider;

  // Justering af hastigheder baseret på proximity-værdierne
  if (ProxHojre < ProxVenstre) {
    motor.setSpeeds(110, -50); //drej mod højre
  } else if (ProxVenstre < ProxHojre) {
    motor.setSpeeds(-50, 110); //drej mod venstre
  }
}


void CheckIfLineDetected(){
  lineSensors.read(lineSensorValues, QTR_EMITTERS_ON);
  
  // Skift til stopstage, hvis linjen er registreret
  if (lineSensorValues[0] > thresh || lineSensorValues[1] > thresh || lineSensorValues[2] > thresh) {
    stage = 1; //stop
    } else {
    stage = 0; //turn
    }
}

void Stop() {
  motor.setSpeeds(0, 0);
  
  // Venter på knap A-tryk for genstart
  while (buttonA.isPressed() != 1) {
    // Gør ingenting, venter på knap A-tryk
  }
  
  delay(1000);
  stage = 0; // Nulstil stadiet for at genstarte processen
}
