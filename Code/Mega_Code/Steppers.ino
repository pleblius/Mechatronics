void stepperSetup() {
  zMotor.setCurrentPosition(0);
  turretMotor.setCurrentPosition(0);
  armMotor.setCurrentPosition(0);

  zMotor.setMaxSpeed(100);
  turretMotor.setMaxSpeed(800);
  armMotor.setMaxSpeed(400);

  zMotor.setAcceleration(100);
  turretMotor.setAcceleration(200);
  armMotor.setAcceleration(200);
}

bool moveNeeded(int curPos, int desPos) {
  if (desPos - curPos <= 1) {
    return false;
  }

  return true;
}