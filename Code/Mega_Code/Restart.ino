/*  Initiates restart of all robot operations. */
void restartService() {
  // Restart code 0 - initiating restart
  sendTransmission('R', 0);

  deactivateMagnet();

  // Reset arm position
  moveToHome();

  // Brake
  wheelBrake();

  // Reset wheel motors
  resetMotors();

  // Reset block queue
  resetQueues();

  state = RESTARTING;
}

/*  Operations while state is restarting. When operations are finished, moves into STARTUP. */
void restartOperations() {
  runSteppers();

  if (finishedStepping()) {
    // Restart code 1 - finished restarting
    sendTransmission('R', 1);

    // Start code 0 - begin startup
    sendTransmission('S', 0);

    state = STARTUP;
  }
}

/*  Checks if a restart command has been received. */
bool restartTriggered() {
  return (rxChar == 'R' || rxChar == 'r');
}