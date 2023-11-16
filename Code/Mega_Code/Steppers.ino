// Stepper Pins
#define ADIR 22
#define ZDIR 30
#define TDIR 34
#define WDIR 26
#define ASTEP 24
#define ZSTEP 32
#define TSTEP 36
#define WSTEP 28

// Stepper characteristics
float stepSize = 1.8;
float stepSizeRad = stepSize*PI/180.;
float stepsPerRev = 360./stepSize;
float degPerRad = 180.0/PI;
float radPerDeg = PI/180.0;

// Step-to-degree ratios
float turretRatio = 7.4;
float armRatio = 3.5;
float zRatio = 140.0;

// Robot parameters (in)
float zHeight = 8;
float armLength = 9.5;
float turretLength = 9.5;

// Angular offsets in default position (degrees)
float turretOffset = 45;
float armOffset = 140;

// Stepper constructurs - Motor mode (Driver = 2-pin), Step-pin, Dir-pin
AccelStepper armMotor = AccelStepper(AccelStepper::DRIVER, ASTEP, ADIR);
AccelStepper turretMotor = AccelStepper(AccelStepper::DRIVER, TSTEP, TDIR);  
AccelStepper zMotor = AccelStepper(AccelStepper::DRIVER, ZSTEP, ZDIR);
AccelStepper wristMotor = AccelStepper(AccelStepper::DRIVER, WSTEP, WDIR);

/*  Initial setup for the three primary stepper motors. Sets their
 *  initial position, maximum speed, and acceleration.
 */
void stepperSetup() {
  zMotor.setCurrentPosition(0);
  turretMotor.setCurrentPosition(0);
  armMotor.setCurrentPosition(0);
  wristMotor.setCurrentPosition(0);

  zMotor.setMaxSpeed(300);
  turretMotor.setMaxSpeed(300);
  armMotor.setMaxSpeed(300);
  wristMotor.setMaxSpeed(25);

  zMotor.setAcceleration(150);
  turretMotor.setAcceleration(150);
  armMotor.setAcceleration(150);
  wristMotor.setAcceleration(10);
}

/*  Runs the stepper motors */
void runSteppers() {
  zMotor.run();
  turretMotor.run();
  armMotor.run();
  wristMotor.run();
}

/*  Returns true if the steppers are done moving, false otherwise. */
bool finishedStepping() {
  return zMotor.distanceToGo() == 0 &&
    armMotor.distanceToGo() == 0 &&
    turretMotor.distanceToGo() == 0 &&
    wristMotor.distanceToGo() == 0;
}

/*  Moves the turret/proximal arm to the desired angle (degrees).
 *  Be aware of potential cable issues on excessive rotation.
 */
void moveTurretToAngle(float angle) {
  turretMotor.moveTo(turretRatio*angle);
}

/*  Moves the distal arm to the desired angle relative to the proximal arm (degrees).
 *  Be aware of potential cable issues on excessive rotation.
 */
void moveArmToAngle(float angle) {
  armMotor.moveTo(armRatio*angle);
}

/*  Moves the wrist joint to the desired fixed angle (degrees).
 *  Be aware of potential cable issues on excessive rotation.
 */
void moveWristToAngle(float angle) {
  wristMotor.moveTo(stepSize*angle);
}

/*  Moves the vertical configuration to the desired position.
 *  Position should be between 0.0 and 1.0, with 0.0 corrsponding to minimum height 
 *  and 1.0 corresponding to maximum height.
 */
void moveZToPos(float pos) {
  zMotor.moveTo(zRatio*pos);
}

/*  Moves the entire SCARA assembly to the given three-dimensional coordinates, and adjusts the wrist-angle to remain neutral.
 *  x, y, and z should be in inches.
 *  WARNING: This method DOES NOT CHECK if the desired position would exceed the reach of the arm.
 *  WARNING: This method DOES NOT CHECK if the desired position would collide with itself.
 */
void moveToPos(float x, float y, float z) {
  float zPos;
  float armAngle;
  float turretAngle;
  
  // Run inverse kinematics to get angles from position
  zPos = z;
  armAngle = getArmAngle(x, y);
  turretAngle = getTurretAngle(x, y, armAngle) + turretOffset;
  armAngle -= armOffset;

  // Move arms to respective angles
  moveZToPos(zPos);
  moveArmToAngle(armAngle);
  moveTurretToAngle(turretAngle);

  // Adjust wrist to remain in neutral plane (aligned with chassis).
  float wristAngle = armAngle + turretAngle;
  moveWristToAngle(wristAngle);
}

/*  Converts the position from cartesian coordinates (in inches) to arm angles theta1 and theta2.
 *  Method will prioritize returning negative angles that are less likely to result in arm collisions
 *  with oourse infrastructure.
 *  This method returns the distal angle theta2 that the distal arm must be rotated to.
 */
float getArmAngle(float x, float y) {
  float x_2 = x*x;
  float y_2 = y*y;
  float l1_2 = turretLength*turretLength;
  float l2_2 = armLength*armLength;
  
  float num = x_2 + y_2 - l1_2 - l2_2;
  float den = 2*armLength*turretLength;

  float retVal = acos(num/den)*degPerRad;

  // Return angle in degrees
  return retVal;
}

/*  Converts the position from cartesian coordinates (in inches) to a turret angle. Requires distal arm angle for
 *  calculation, in degrees.
 *  Method will prioritize returning large positive angles that are less likely to result in arm collisions
 *  with oourse infrastructure.
 */
float getTurretAngle(float x, float y, float q2) {
  float num, den;
  float q2_rad = q2*radPerDeg;
  
  num = armLength*sin(q2_rad);
  den = turretLength + armLength*cos(q2_rad);
  
  float retVal = atan2(y, x)*degPerRad - atan2(num, den)*degPerRad;

  return retVal;
}

/*  Moves arms to starting position.
 *  WARNING: This method is open-loop. Starting position is based on position when Arduino was started.
 */
void moveToHome() {
  zMotor.moveTo(0);
  armMotor.moveTo(0);
  turretMotor.moveTo(0);
  wristMotor.moveTo(0);
}