#include <Stepper.h>

const int stepsPerRev = 12800; //roughly

int startButtonPin = 7;
int startButtonState = 1; //not really necessary but shows normal state of switch

int limitSwitchTopPin = 6;
int limitSwitchTopState = 1;

int limitSwitchYPin = 8;
int limitSwitchYState = 1;

boolean motorRunX = true; //true will allow steppers to run
char motorDirectionX = 'R'; //from top view. R = RIGHT = DOWN
double positionX = 0;  //poisiton in number of rotations from starting point (rough)

boolean motorRunY = true; //true will allow steppers to run
char motorDirectionY = 'R'; //from top view. R = RIGHT = away from motor (positive position)
double positionY = 0;  //poisiton in number of rotations from starting point (rough)

boolean motorRunZ = true; //true will allow steppers to run
char motorDirectionZ = 'R'; //from top view. R = idk
double positionZ;  //poisiton in number of rotations from starting point (rough)

int flag = 0; //0:do not run 1:run in first sample mode, additional values will indicate different running modes

int numSheetsStacked = 0;


Stepper stepperX(stepsPerRev, 3, 2); //X steppers
Stepper stepperY(stepsPerRev, 5, 4); //Y steppers
Stepper stepperZ(stepsPerRev, 10, 9); //Z stepper

void setup() {
  stepperX.setSpeed(1000); //like 25 rpm
  stepperY.setSpeed(1000);
  stepperZ.setSpeed(20);
  Serial.begin(9600);
  pinMode(7, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);
  pinMode(8, INPUT_PULLUP);
  initializeY();
  initializeX();
  positionZ = 0;
  delay(500);
  readSwitches();
  while (startButtonState == 1) {
    Serial.println("Place Sheet and Press Start");
    readSwitches();
  }
}

void loop() {
  if (flag == 1) {
    readSwitches();
    updateControlsX();
    moveMotorX();
    Serial.println("case 1");
  }
  if (flag == 2) {
    readSwitches();
    updateControlsY();
    if (flag == 2){
      moveMotorY();
      Serial.println("case 2");
    }
  }
  if (flag == 3) {
    stack();
    Serial.println("case 3");
  }
  if (flag == 4) {
    readSwitches();
    while (startButtonState == 1) {
      readSwitches();
      Serial.println("waiting");
    }
    initializeY();
    initializeX();
    readSwitches();
    while (startButtonState == 1) {
      readSwitches();
      Serial.println("waiting");
    }
    Serial.println("case 4");
    flag = 1;
  }
}

void stack() {
  motorDirectionX = 'R';
  motorRunX = true;
  while (positionX >= -20) {
    moveMotorX();
  }
  motorDirectionZ = 'R';
  positionZ = 0;
  while (positionZ <= 0.74) {
    moveMotorZ();
  }
  while (positionX >= (-65 + (0.5 * numSheetsStacked))) {
    moveMotorX();
  }
  readSwitches();
  while(startButtonState == 1) {
    Serial.println("waiting");
    readSwitches();
  }
  motorDirectionX = 'L';
  while (positionX <= -20) {
    moveMotorX();
  }
  motorDirectionZ = 'L';
  while (positionZ > 0) {
    moveMotorZ();
  }
  while (positionX <= -5) {
    moveMotorX();
  }
  numSheetsStacked = numSheetsStacked + 1;
  flag = 4;
}


void updateControlsX() {
  if (positionX >= -1) { //positional placeholder value (15), calculate position where rig should stop for gluing
    motorRunX = false;
    readSwitches();
    while (startButtonState == 1) {
      Serial.println("press to begin gluing");
      readSwitches();
    }
    flag = 2;
  }
}

void updateControlsY() {
  if (positionY >= 22) { //positional placeholder value (15), calculate position where rig should stop for gluing
    motorRunY = false;
    readSwitches();
    while (startButtonState == 1) {
      Serial.println("switching to state 3 on button press");
      readSwitches();
    }
    flag = 3;
  }
}

void readSwitches(){
  startButtonState = digitalRead(startButtonPin);
  //Serial.println(limitSwitchStateFar);

  limitSwitchTopState = digitalRead(limitSwitchTopPin);
  //Serial.println(limitSwitchStateNear);

  limitSwitchYState = digitalRead(limitSwitchYPin);
  //Serial.println(limitSwitchStateNear);
}

void moveMotorX(){
  if (motorRunX = true) {
    if (motorDirectionX == 'R') {
      stepperX.step(-(stepsPerRev/20));
      positionX = positionX - 0.025;
      Serial.println(positionX);
    } else {
      stepperX.step(stepsPerRev/20);
      positionX = positionX + 0.025; 
      Serial.println(positionX);
    }
  } 
}

void moveMotorY(){
  if (motorRunY = true) {
    if (motorDirectionY == 'R') {
      stepperY.step(-(stepsPerRev/20));
      positionY = positionY + 0.025;
      Serial.println(positionY);
    } else {
      stepperY.step(stepsPerRev/20);
      positionY = positionY - 0.025; 
      Serial.println(positionY);
    }
  }
}

void moveMotorZ(){
  if (motorRunZ = true) {
    if (motorDirectionZ == 'R') {
      stepperZ.step(-(stepsPerRev/20));
      positionZ = positionZ + 0.025;
      Serial.println(positionZ);
    } else {
      stepperZ.step(stepsPerRev/20);
      positionZ = positionZ - 0.025; 
      Serial.println(positionZ);
    }
  }
}

void initializeX(){
  while (startButtonState == 1) {
    Serial.println("waiting for input to initialize X");
    readSwitches();
  }
  if (startButtonState == 0){
    motorRunX = true;
    Serial.println("HOMING");
    //flag = 1;
    motorDirectionX = 'L';
    Serial.println("Begin X Initialization");
    delay(20);
    while(limitSwitchTopState == 1) {
      readSwitches();
      moveMotorX();
    }
    delay(500);
    motorDirectionX = 'R';
    moveMotorX();
    moveMotorX();
    moveMotorX();
    positionX = 0;
    while (positionX >= -15) {
      moveMotorX();
    }
    delay(500);
    flag = 1;
    motorDirectionX = 'L';
  }
}

void initializeY(){
  while (startButtonState == 1) {
    Serial.println("waiting for input to initialize Y");
    readSwitches();
  }
  motorRunY = true;
  if (startButtonState == 0){
    Serial.println("HOMING");
    motorDirectionY = 'L';
    Serial.println("Begin Y Initialization");
    delay(20);
    while(limitSwitchYState == 1) {
      readSwitches();
      moveMotorY();
    }
    delay(500);
    motorDirectionY = 'R';
    moveMotorY();
    moveMotorY();
    moveMotorY();
    stepperY.setSpeed(600);
    positionY = 0;
  }
}