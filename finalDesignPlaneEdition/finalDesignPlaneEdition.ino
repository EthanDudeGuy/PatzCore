#include <Stepper.h>

const int stepsPerRev = 12800; //roughly
const int stepsToTurnBed = ?;

//switches
int limitSwitchPinFarY = 7;
int limitSwitchStateFarY; //not really necessary but shows normal state of switch

int limitSwitchPinNearY = 6;
int limitSwitchStateNearY;

int limitSwitchPinFarX = 5;
int limitSwitchStateFarX; //not really necessary but shows normal state of switch

int limitSwitchPinNearX = 4; //near = bottom for vertical
int limitSwitchStateNearX;

int startButtonPin = 10; //placeholder pin
int startButtonState;

int eStopPin = 12; //placeholder pin
int eStopState;

int glueActivatePin = 12; //placeholder pin
int glueActivateState;

int lidarPin = 13;  //placeholder
double lidarState;

//motor control vars
boolean motorRunX = false; //true will allow steppers to run
char motorDirectionX = 'R'; //from top view. R = RIGHT = up (with NEMA on bottom)

boolean motorRunY = false; //true will allow steppers to run
char motorDirectionY = 'R'

boolean motorRunZ = false; //true will allow steppers to run
char motorDirectionZ = 'R'

double positionY; //vertical
double positionX; //horizontal
double positionZ; //rotation

//flags + other state controls
int flag = 0;
int flagMode;

//stepper initialization
Stepper stepperX(stepsPerRev, 3, 2);
Stepper stepperY(stepsPerRev, ?, ?);  //undefined pins 
Stepper stepperZ(stepsPerRev, ?, ?);  //undefined pins 

void setup() {
  stepperX.setSpeed(20); //slow initially for homing sequence
  Serial.begin(9600);
  pinMode(7, INPUT_PULLDOWN);
  pinMode(6, INPUT_PULLDOWN);
  pinMode(5, INPUT_PULLDOWN);
  pinMode(4, INPUT_PULLDOWN);
  pinMode(10, INPUT_PULLDOWN);
  pinMode(11, INPUT_PULLDOWN);
  pinMode(12, INPUT_PULLDOWN);
  initializeX();
  delay(500);
  stepperX.setSpeed(600); //operation speed for normal run
}

void loop() {
  switch (flag) {
    case 1: //occurs immediately after setup, moves plate to gluing position
      if (flag == 1) {
        readSwitches();
        updateControlsX();
        if (flag == 1) {
          moveMotorX();
        }
      }
    case 2:  //gluing
      readSwitches();
      updateControlsY();
      if (glueActivateState == 1 && flag ==2) {
        moveMotorY();
      }
    case 3:
      stack();
      initializeX();
      delay(500);
      stepperX.setSpeed(600);
    default:
      break;
  }
}

void stack() {
  if (flag == 3) {
    for (int i = 0, i < 4, i++) {
      moveMotorX();
    }
    stepperZ.step(stepsToTurnBed);
    while(lidarState > 6 && positionX > 0) { //lidar is still a certain distance above stack, want it to stop moving down when a constant distance above
      readSwitches();
      moveMotorX();
    } 
    readSwitches();
    while(startButtonState == 0) {
      Serial.println("WAITING FOR BUTTON PRESS");
      readSwitches();
    }
    motorDirectionX = 'R';
    for (int i = 0, i < 4, i++) {
      moveMotorX();
    }
    stepperZ.step(-(stepsToTurnBed));
    flag = 0;
  }
}


void updateControlsX() {
  if (eStopState == 1) {
    Serial.println("EMERGENCY STOP TRIGGERED");
    flag = 0;
    motorRunY = false;
    motorRunX = false;
    motorRunZ = false;
  }
  if (position >= 15 && flag == 1) { //positional placeholder value (15), calculate position where rig should stop for gluing
    motorRunX = false;
    readSwitches();
    while (startButtonState == 0) {
      Serial.println("WAITING FOR BUTTON PRESS");
      readSwitches();
    }
    initializeY();
    flag = 2;
  }
}

void updateControlsY() {
  if (eStopState == 1) {
    Serial.println("EMERGENCY STOP TRIGGERED");
    flag = 0;
    motorRunY = false;
    motorRunX = false;
    motorRunZ = false;
  }
  if (position >= 15) { //placeholder position
    motorRunY = false;
    readSwitches();
    while(startButtonState == 0) {
      Serial.println("WAITING FOR BUTTON PRESS");
      readSwitches();
    } 
    motorRunX = true;
    motorDirectionX = 'L';
    motorRunZ = true;
    motorDirectionZ = 'set later********';
    flag = 3; //stacking
  }
}

void initializeX() {
  while (flag == 0) {
    Serial.println("WAITING FOR BUTTON PRESS");
    readSwitches();
    if (startButtonState == 1) {
      Serial.println("HOMING, PLEASE WAIT");
      motorDirectionX = 'L'; //move down when run is called
      motorRunX = true;
      delay(20);
      readSwitches();
      while(limitSwitchNearX == 0 && eStopState == 0) { //move down until limit switch pressed
        if (eStopState == 1) {
          Serial.println("EMERGENCY STOP TRIGGERED");
          motorRunY = false;
          motorRunX = false;
          motorRunZ = false;
          flag = 0;
        }
        readSwitches();
        moveMotorX();
      }
      delay(500);
      motorDirectionX = 'R';
      readSwitches();
      if (eStopState == 0) {
        stepperX.step(-(stepsPerRev));
      }
      positionX = 0;
      Serial.println("POSITION 0 SET");
      moveMotor();
      delay(500);
      flag = 1;
    }
  }
}

void initializeY() {
  motorDirectionY = 'L';
  motorRunY = true;
  stepperY.setSpeed(20);
  readSwitches();
  Serial.println("HOMING Y")
  while(limitSwitchStateNearY == 0 && eStopState == 0) {
    if (eStopState == 1) {
      Serial.println("EMERGENCY STOP TRIGGERED");
      motorRunY = false;
      motorRunX = false;
      motorRunZ = false;
      flag = 0;
    }
    readSwitches();
    moveMotorY();
  }
  delay(500);
  motorDirectionY = 'R';
  if (eStopState == 0) {
    stepperX.step(-(stepsPerRev));
  }
  positionY = 0;
  Serial.println("Y POSITION 0 SET")
  moveMotorY();
  stepperY.setSpeed(600);
  delay(500);
}

void moveMotorX() {
  if (motorRunX = true) {
    if (motorDirectionX == 'R') {
      stepperX.step(-(stepsPerRev/20));
      positionX = positionX + 0.025;;
      Serial.println(positionX);
    } else {
      stepperX.step(stepsPerRev/20);
      positionX = positionX - 0.025; 
      Serial.println(positionX);
    }
  }
}

void moveMotorY() {
  if (motorRunY = true) {
    if (motorDirectionY == 'R') {
      stepperY.step(-(stepsPerRev/20));
      positionY = positionY + 0.025;;
      Serial.println(positionY);
    } else {
      stepperX.step(stepsPerRev/20);
      positionY = positionY - 0.025; 
      Serial.println(positionY);
    }
  }
}

void readSwitches() {
  limitSwitchStateFarY = digitalRead(limitSwitchPinFarY);
  //Serial.println(limitSwitchStateFarY);

  limitSwitchStateNearY = digitalRead(limitSwitchPinNearY);
  //Serial.println(limitSwitchStateNearY);

  limitSwitchStateFarX = digitalRead(limitSwitchPinFarX);
  //Serial.println(limitSwitchStateFarX);

  limitSwitchStateNearX = digitalRead(limitSwitchPinNearX);
  //Serial.println(limitSwitchStateNearX);

  startButtonState = digitalRead(startButtonPin);
  //Serial.println(startButtonState);

  eStopState = digitalRead(eStopPin);
  //Serial.println(eStopState);

  glueActivateState = digitalRead(glueActivatePin);
  //Serial.println(glueActivateState);
}