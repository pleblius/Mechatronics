/*  Initiates restart of all robot operations. */
void restartService() {
  // Restart code 0 - initiating restart
  sendTransmission('R', 0);

  // Reset arm position
  moveToHome();

  // Brake
  wheelBrake();

  // Reset block queue
  resetQueues();

  state = RESTARTING;
}

/*  Operations while state is restarting. When operations are finished, moves into STARTUP. */
void restartOperations() {
  runSteppers();

  if (finishedStepping()) {
    // Reset wheel motors
    resetMotors();

    deactivateMagnet();
    
    // Restart code 1 - finished restarting
    sendTransmission('R', 1);

    // Start code 0 - begin startup
    sendTransmission('S', 5);

    state = STARTUP;
  }
}

/*  Checks if a restart command has been received. */
bool restartTriggered() {
  return (rxChar == 'R' || rxChar == 'r');
}