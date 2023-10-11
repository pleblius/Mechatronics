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