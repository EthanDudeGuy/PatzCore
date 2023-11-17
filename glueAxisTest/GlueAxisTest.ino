#include <Stepper.h>

const int stepsPerRev = 12800; //roughly

int limitSwitchPinFar = 7;
int limitSwitchStateFar = 1; //not really necessary but shows normal state of switch

int limitSwitchPinNear = 6;
int limitSwitchStateNear = 1; //not really necessary but shows normal state of switch

boolean motorRun = true; //true will allow steppers to run
char motorDirection = 'R'; //from top view. R = RIGHT = up (with NEMA on bottom)
double position = 0;  //poisiton in number of rotations from starting point (rough)

int flag = 0; //0:do not run 1:run in first sample mode, additional values will indicate different running modes
int flagMode = 0; //variable to store desired run starting point

int count = 0;


Stepper stepperY(stepsPerRev, 3, 2);

void setup() {
  stepperY.setSpeed(20); //like 25 rpm
  Serial.begin(9600);
  pinMode(7, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);
  initialize();
  delay(500);
  stepperY.setSpeed(600);
}

void loop() {
  while (flag == 1 && position >= 0) {
    readSwitches();
    updateControls();
    moveMotor();
  } 
  Serial.println("RUN COMPLETE");
}

void readSwitches(){
  limitSwitchStateFar = digitalRead(limitSwitchPinFar);
  //Serial.println(limitSwitchStateFar);

  limitSwitchStateNear = digitalRead(limitSwitchPinNear);
  //Serial.println(limitSwitchStateNear);
}

void updateControls(){
  if (limitSwitchStateFar == 0){
    Serial.println("EMERYGENCY STOP");
    flag = 0;
  }
  if (position >= 15) { // position comparison determines run distance NEVER LONGER THAN 20
    if (motorDirection == 'R'){
      motorRun = true;
      motorDirection = 'L';
      delay(20);
      while(digitalRead(limitSwitchPinFar) == 0){
        moveMotor();
      }
    }  
  }
  if (limitSwitchStateNear == 0){
    if (motorDirection == 'L'){
      motorRun = true;
      motorDirection = 'R';
      delay(20);
      while(digitalRead(limitSwitchPinNear) == 0){
        moveMotor();
      }
      stepperY.step(-(stepsPerRev)); //doesn't increment position but moves away from switch before next initialization and run
      flag = 0;
    }
  }
}

void moveMotor(){
  if (motorRun = true) {
    if (motorDirection == 'R') {
      stepperY.step(-(stepsPerRev/20));
      position = position + 0.025;;
      Serial.println(position);
    } else {
      stepperY.step(stepsPerRev/20);
      position = position - 0.025; 
      Serial.println(position);
    }
  } 
}

void initialize(){
  while (flag == 0) {
    Serial.println("WAITINGFORFLAG");
    readSwitches();
    if (limitSwitchStateFar == 0){
      Serial.println("HOMING");
      flag = 1;
      motorDirection = 'L';
      Serial.println("BEGIN SEQUENCE 1");
      delay(20);
      while(limitSwitchStateNear == 1) {
        readSwitches();
        moveMotor();
      }
      delay(500);
      motorDirection = 'R';
      stepperY.step(-(stepsPerRev));  
      position = 0;
      moveMotor();
      delay(500);
    }
  }
}