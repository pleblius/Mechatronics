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
float degPerRad = 180.0/pi;
float radPerDeg = pi/180.0;

// Step-to-degree ratios
float turretRatio = 95.0;
float armRatio = 50.0;
float zRatio = 80.0;

// Robot parameters (in)
float zHeight;
float armLength = 9.5;
float turretLength = 9.5;

// Angular offsets in default position (degrees)
float turretOffset = 45;
float armOffset;

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

/*  Returns true if the steppers are done moving, false otherwise. */
bool finishedStepping() {
  return zMotor.distanceToGo() == 0 &&
    armMotor.distanceToGo() == 0 &&
    turretMotor.distanceToGo() == 0;
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
  zPos = getZPos(z);
  armAngle = getArmAngle(x, y) + armOffset;
  turretAngle = getTurretAngle(x, y, armAngle) + turretOffset;

  // Move arms to respective angles
  moveZToPos(zPos);
  moveArmToAngle(armAngle);
  moveTurretToAngle(turretAngle);

  // Adjust wrist to remain in neutral plain (aligned with chassis).
  float wristAngle = armAngle + turretAngle;
  // TODO - IMPLEMENT SERVO CONTROLS FOR WRIST
}

/*  Converts the z position from inches to a ratio of max height. */
float getZPos(float z) {
  return z/zHeight;
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

  // Return angle in degrees
  return -acos(num/den)*degPerRad;
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

  return atan2(y, x)*degPerRad + atan2(num, den)*degPerRad;
}

/*  Moves arms to starting position. Used when RESET is called or competition is over.
 *  WARNING: This method is open-loop. Starting position is based on position when Arduino was started.
 */
void resetArms() {
  moveToPos(0, 0, 0);
}