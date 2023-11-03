#include <AccelStepper.h>

// Define the stepper motor and the pins that is connected to
AccelStepper stepper1(2, 3); // (Typeof driver: with 2 pins, STEP, DIR)
int step = 0;


void setup() {
  stepper1.setMaxSpeed(1000); // Set maximum speed value for the stepper
  stepper1.setAcceleration(500); // Set acceleration value for the stepper
  stepper1.setCurrentPosition(0); // Set the current position to 0 steps
}


void loop() {
  step = step + 1600;
  while (stepper1.currentPosition() != 0) {
    step = step++;
    stepper1.moveTo(step);
    stepper1.runToPosition();
  }

  // Move back to position 0, using run() which is non-blocking - both motors will move at the same time
  //stepper1.moveTo(0);
  //stepper2.moveTo(0);
  //while (stepper1.currentPosition() != 0 || stepper2.currentPosition() != 0) {
    //stepper1.run();  // Move or step the motor implementing accelerations and decelerations to achieve the target position. Non-blocking function
    //stepper2.run();
    //
    //
}
