#include <Wire.h>                    //disse er nødvendige for at programmet kan køre
#include <Zumo32U4.h>

int thresHold=700;

Zumo32U4Motors motors;                        // nødvendig for at zumoen kan køre


Zumo32U4LineSensors lineSensors;               // nødvendig for at læse sort tape
#define NUM_SENSORS 5                           //definere størrelse af listen hvor data bliver gemt
uint16_t lineSensorValues[NUM_SENSORS];         // liste hvor data bliver gemt
uint16_t surfaceBrightnes[NUM_SENSORS] = {240,170,140,170,240};
uint8_t lineTreshold = 300;




Zumo32U4ButtonA buttonA;            // for at zumoen ikke kører i det øjeblik den bliver tændt(bruger buttonA til at sætte igang)

void printReadinToSerial(){
  char buffer[80];
  sprintf(buffer,"%4d %4d %4d %4d %4d\n",
  lineSensorValues[0],
  lineSensorValues[1],
  lineSensorValues[2],
  lineSensorValues[3],
  lineSensorValues[4]);
  Serial.print(buffer);
}                      
                                                                    
  
void readLineSensors(){       //læser værdier
  lineSensors.read(lineSensorValues, QTR_EMITTERS_ON);   // læser værdier 0=lys  2000=mørk
} 

void driveToTape(){
  motors.setSpeeds(70,70);
  while (true){
    lineSensors.read(lineSensorValues, QTR_EMITTERS_ON);
    if (lineSensorValues[2]>surfaceBrightnes[2]+lineTreshold){
    break;
    }
    else if (lineSensorValues[1]>surfaceBrightnes[1]+lineTreshold){
      break;
    }
    else if (lineSensorValues[3]>surfaceBrightnes[3]+lineTreshold){
      break;
        
    }
    else if (lineSensorValues[0]>surfaceBrightnes[0]+lineTreshold){
      break;
    }
    else if (lineSensorValues[4]>surfaceBrightnes[4]+lineTreshold){
      break;
    }
  }
  motors.setSpeeds(0,0);
}

void alignVithTape(){ 
  printReadinToSerial();      //kør frem, find tape, juster så værdier er ens, tilbage, juster så værdier er ens
  driveToTape();
 lineSensors.read(lineSensorValues, QTR_EMITTERS_ON);   // læser værdier 0=lys  2000=mørk
 printReadinToSerial();
    if (lineSensorValues[2]>surfaceBrightnes[2]+lineTreshold){
      motors.setSpeeds(0,0);
      if (lineSensorValues[0]>surfaceBrightnes[0]+lineTreshold && lineSensorValues[4]>surfaceBrightnes[4]+lineTreshold){
        return;
      }
      else if (lineSensorValues[3]>surfaceBrightnes[3]+lineTreshold && lineSensorValues[1]<surfaceBrightnes[1]+lineTreshold){
        motors.setSpeeds(50,-50);
      }
      else if (lineSensorValues[1]>surfaceBrightnes[1]+lineTreshold && lineSensorValues[3]<surfaceBrightnes[3]+lineTreshold){
        motors.setSpeeds(-50,50);
      }
      delay(500);
    }
    else if (lineSensorValues[1]>surfaceBrightnes[1]+lineTreshold){
      motors.setSpeeds(-50,50);
      delay(50);
    }
    else if (lineSensorValues[3]>surfaceBrightnes[3]+lineTreshold){
      motors.setSpeeds(50,-50);
      delay(50);
        
    }
    else if (lineSensorValues[0]>surfaceBrightnes[0]+lineTreshold){
      motors.setSpeeds(-50,50);
      delay(100);
    }
    else if (lineSensorValues[4]>surfaceBrightnes[4]+lineTreshold){
      motors.setSpeeds(50,-50);
      delay(50);
    }
       
    motors.setSpeeds(0,0);
    
  
}


 
void setup() {               // setup kører en gang
  Serial.begin(9600);                  // komunikations hastighed
  lineSensors.initFiveSensors();                   // bruger de fem sensore i front  bruges i setup en enkelt gang
  buttonA.waitForButton();   // venter på et tryk på buttonA
  motors.setSpeeds(70,70);
  for (int i=0; i<100; i++){
    alignVithTape();
  }
  delay(1000);
  motors.setSpeeds(0,75);
  delay(1200);
  alignVithTape();
  
}


void loop() {                                           // kører loopet
  
  //display.clear();                                      //display klar
  motors.setSpeeds(0,0);                            // hastighed
  
}
