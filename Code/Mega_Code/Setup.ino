/*  Sets state to SETUP mode. */
void setupService() {
  state = SETUP;
}

/*  Operations while setting up competition.
 *  Gets user input to build/update queue.
 *  Checks for restart commands. */
void setupOperations() {
  if (receiveTransmission()) {
    if (restartTriggered()) {
      restartService();
    }
    // If invalid block location, exit block early
    else if (rxInt >= 20) {
      // Block code 19 - invalid block number
      sendTransmission('B', 19);
    }
    // If queue hasn't been built yet, check for valid-queue build command and initiate build
    else if (!isQueueBuilt()) {
      setupQueue();

      // Start Code 1 - Prompt user for start command
      sendTransmission('S', 1);

      // Block Code 1 - Prompt user for additional blocks
      sendTransmission('B', 1);

      printQueues();
    }
    // If queue is already built, check for start command
    else if (startTriggered()) {
      startCompetition();
    }
    // If queue is already built, check for update command
    else if (updateTriggered()) {
      updateQueue(rxInt);

      // Block Code 1 - Prompt user for additional blocks
      sendTransmission('B', 1);

      printQueues();
    }
    else {
      // Error code 1 - unknown command received
      sendTransmission('X', 1);
    }
  }
}

/*  Uses the cached rxChar and rxInt to create a new queue, with the random starting block
 *  determined by rxChar at position rxInt. */
void setupQueue() {
  switch (rxChar) {
    case 'W':
    case 'w': {
      buildQueue(WHEEL, rxInt);
    } break;

    case 'F':
    case 'f': {
      buildQueue(FAN, rxInt);
    } break;

    case 'B':
    case 'b': {
      buildQueue(BATTERY, rxInt);
    } break;

    default: {
      // Error code 19 - invalid block type
      sendTransmission('X', 19);
    }
  }
}

/*  Returns true if the user has sent a START command. */
bool startTriggered() {
  if (rxChar == 's' || rxChar == 'S') {
    return true;
  }

  return false;
}

/*  Returns true if the user has sent a queue-update command. */
bool updateTriggered() {
  if (rxChar == 'a' || rxChar == 'A') {
    return true;
  }

  return false;
}

/*  Starts the competition by initiating line-following. */
void startCompetition() {
  // Start Code 2 - Inform user competition is starting
  sendTransmission('S', 2);
  state = EXITING;
}