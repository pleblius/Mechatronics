void loadingService() {
  nextPoint = getBlockCatchUp();
  moveToNextPoint();

  state = LOADING;
}

/*  Services to perform when initiating a block discard. */
void placingService() {
  generateApproach(blockPosition);
  state = PLACING;
}

/*  Operations to perfrom while discarding a block. */
void placingOperations() {
  runSteppers();

  if (finishedStepping()) {
    // Finished approaching - block should be in discard position
    if (finishedApproaching()) {
      // Turn off magnet and begin returning arms
      deactivateMagnet();
      approachReturn();

      // Initiate movement to next return point
      nextPoint = getNextPoint();
      moveToNextPoint();
    }
    // Finished returning - arms should be at home position
    else if (finishedReturning()) {
      // Check if placement queue is exhausted == competition finished
      if (exhausted()) {
        restartService();
      }
      // Check if block was a discard or legit placement
      else if (discard) {
        // If block was a discard, get a new block
        acquiringService();
      } else {
        // If block was a valid placement, begin returning to block dispenser
        nextPoint = homePoint;
        moveToNextPoint();
        state = FORWARD;
      }
    }
    // Get next point 
    else {
      nextPoint = getNextPoint();
      moveToNextPoint();
    }
  }

  restartCheck();
}