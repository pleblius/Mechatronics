/*  Robot operation while traveling in reverse from block dispenser to vehicle chassis. */
void backingOperations() {
  static bool firstLeg = true;
  static bool turning = false;
  float speed = 0.5;

  if (!finishedStepping()) {
    runSteppers();
  }
  
  // If turning, use PID control
  if (turning) {
    PIDDrive();

    if (atDestination()) {
      turning = false;
      firstLeg = false;
    }
  }
  // If still on first part of leg, line follow until intersection
  else if (firstLeg) {
    // When intersection is reached, turn to follow branch
    if (atIntersectionRear()) {
      wheelBrake();
      turning = true;
      rotate(85, 5);
    } else {
      reverseFollow(.3);
    }
  }
  // If on second leg, line follow for desired distance
  else {
    if (getRearDistance() < desReverseDistance) {
      wheelBrake();
      loadingService();
      firstLeg = true;
    } else {
      reverseFollow(.4);
    }
  }

  restartCheck();
}

/*  Robot operation while traveling in reverse from block dispenser to vehicle chassis. */
void forwardOperations() {
  static bool firstLeg = true;
  static bool turning = false;
  float speed = 0.4;
  float distance = getFrontDistance();

  if (!finishedStepping()) {
    runSteppers();
  }
  
  // If turning, use PID control
  if (turning) {
    PIDDrive();

    if (atDestination()) {
      turning = false;
      firstLeg = false;
    }
  }
  // If still on first part of leg, line follow until intersection
  else if (firstLeg) {
    // When intersection is reached, turn to follow branch
    if (atIntersectionFront()) {
      turning = true;
      rotate(-82, 5);
    } else {
      forwardFollow(speed);
    }
  }
  // Second leg, approach button
  else {
    // Robot has closed the distance - set state to acquiring
    if (distance < desForwardDistance) {
      acquiringService();
      firstLeg = true;
      return;
    }
    // Robot is closing on button - slow approach
    else if (distance < 5.0) {
      speed = (distance - desForwardDistance)*0.2;
    }
    // Normal line-follow
    else {
      speed = 0.3;
    }

    forwardFollow(speed);
  }

  restartCheck();
}