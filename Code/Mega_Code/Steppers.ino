// Stepper Pins
#define ADIR 33
#define ZDIR 31
#define TDIR 29
#define ASTEP 27
#define ZSTEP 25
#define TSTEP 23

// Stepper characteristics
float stepSize = 1.8;                       
float stepSizeRad = stepSize*pi/180.;       
float stepsPerRev = 360./stepSize;

float turretRatio = 95.0;
float armRatio = 50.0;
float zRatio = 80.0;

// Stepper constructurs - Motor mode (Driver = 2-pin), Step-pin, Dir-pin
AccelStepper armMotor = AccelStepper(AccelStepper::DRIVER, ASTEP, ADIR);
AccelStepper turretMotor = AccelStepper(AccelStepper::DRIVER, TSTEP, TDIR);  
AccelStepper zMotor = AccelStepper(AccelStepper::DRIVER, ZSTEP, ZDIR);

/*  Initial setup for the three primary stepper motors. Sets their
 *  initial position, maximum speed, and acceleration.
 */
void stepperSetup() {
  zMotor.setCurrentPosition(0);
  turretMotor.setCurrentPosition(0);
  armMotor.setCurrentPosition(0);

  zMotor.setMaxSpeed(2000);
  turretMotor.setMaxSpeed(4000);
  armMotor.setMaxSpeed(4000);

  zMotor.setAcceleration(1000);
  turretMotor.setAcceleration(2000);
  armMotor.setAcceleration(2000);
}

/*  Runs the stepper motors */
void runSteppers() {
  zMotor.run();
  turretMotor.run();
  armMotor.run();
}

bool finishedStepping() {
  return zMotor.distanceToGo() == 0 &&
    armMotor.distanceToGo() == 0 &&
    turretMotor.distanceToGo() == 0;
}

/*  Moves the turret/proximal arm to the desired angle.
 *  Be aware of potential cable issues on excessive rotation.
 */
void moveTurretToAngle(float angle) {

}

/*  Moves the distal arm to the desired angle relative to the proximal arm.
 *  Be aware of potential cable issues on excessive rotation.
 */
void moveArmToAngle(float angle) {

}

/*  Moves the vertical configuration to the desired position.
 *  Position should be between 0.0 and 1.0, with 0.0 corrsponding to minimum height and 1.0 corresponding to maximum height.
 */
void moveZToPos(float pos) {

}

/*  Moves the entire SCARA assembly to the given three-dimensional coordinates, and adjusts the wrist-angle to remain neutral.
 *  WARNING: This method DOES NOT CHECK if the desired position would exceed the reach of the arm.
 *  WARNING: This method DOES NOT CHECK if the desired position would collide with itself.
 */
void moveToPos(float x, float y, float z) {

}

/*  Moves arms to starting position. Used when RESET is called or competition is over.
 *  WARNING: This method is open-loop. Starting position is based on position when Arduino is started.
 */
void resetArms() {
  moveToPos(0, 0, 0);
}