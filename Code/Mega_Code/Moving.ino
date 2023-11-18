/*  Robot operation while traveling in reverse from block dispenser to vehicle chassis. */
void backingOperations() {
  static bool firstLeg = true;
  static bool turning = false;
  float speed = 0.4;
  
  // If turning, use PID control
  if (turning) {
    wheelDrive();

    if (atDestination()) {
      turning = false;
      firstLeg = false;
    }
  }
  // If still on first part of leg, line follow until intersection
  else if (firstLeg) {
    // When intersection is reached, turn to follow branch
    if (atIntersectionRear()) {
      turning = true;
      turn(-2, -3);
    } else {
      reverseFollow(speed);
    }
  }
  // If on second leg, line follow for desired distance
  else {
    // Once front line-follower triggers on the intersection, reset distance tracking
    if (atIntersectionFront()) {
      trackDistance(desReverseDistance);
    }
    // Travel for set distance
    else {
      // If distance is reached, brake and begin block placement
      if (getTrackDistance() > desReverseDistance) {
        wheelBrake();
        loadingService();
      }
      // Line follow
      else {
        reverseFollow(speed);
      }
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
  
  // If turning, use PID control
  if (turning) {
    wheelDrive();

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
      turn(2, 3);
    } else {
      forwardFollow(speed);
    }
  }
  // Second leg, approach button
  else {
    // Robot has closed the distance - set state to acquiring
    if (distance < desForwardDistance) {
      acquiringService();
      return;
    }
    // Robot is closing on button - slow approach
    else if (distance < 5.0) {
      speed = (distance - desForwardDistance)*0.2;
    }
    // Normal line-follow
    else {
      speed = 0.4;
    }

    forwardFollow(speed);
  }

  restartCheck();
}