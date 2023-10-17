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