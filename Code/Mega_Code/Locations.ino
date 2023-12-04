Point blockCatchUp = {2, -6, 4.5};
Point blockCatchDown = {2, -6, 1.5};
Point staging = {2, 2, 4.5};
Point discardUp = {10, -6, 4.5};
Point discardDown = {10, -6, 0};
Point homePoint = {4, -4, 4.5};
Point chassisCenter = {3.5, 0, 0.25};

int approachLength = 0;
Point approachArray[10];
int approachIndex = 0;
int approachDir = 1;

/*  Uses the provided block position to generate a new approach/return array.
 *  Array positions are intended to allow arm full movement around any potential blocks that have
 *  already been placed on the chassis.
 *  Array is adjusted based on distance traveled before stopping to place the block. 
 */
void generateApproach(int pos) {
  // If block is useless, discard-in-place
  if (isDiscard(pos)) {
    discardApproach();
    return;
  }

  chassisCenter.y = getRearDistance() + 3.0 + 2.0;

  approachArray[0] = getStaging();

  if (isOnTop(pos)) {
    topSideApproach(pos);
  } else if (isFarSide(pos)) {
    farSideApproach(pos);
  } else if (isLeftSide(pos)) {
    leftSideApproach(pos);
  } else if (isRightSide(pos)) {
    rightSideApproach(pos);
  } else if (isCloseSide(pos)) {
    closeSideApproach(pos);
  }
}

/*  Sets the approach array to the beginning of the array and generates
 *  the approach points based on the position. 
 */
void approachPosition() {
  approachIndex = 0;
  approachDir = 1;
}

/*  Utilizes the already-generated approach array to return to the home position after a block is placed.
 */
void approachReturn() {
  approachDir = -1;
  approachIndex = approachLength - 1;
}

/*  Gets the next point of form (x,y,z) of the approach array, based on whether the arm
 *  is approaching the placement point or the staging location.
 *  Returns an empty point if the array index is out of bounds. 
 */
Point getNextPoint() {
  if (approachIndex >= approachLength) {
    return homePoint;
  } else if (approachIndex < 0) {
    return homePoint;
  }

  Point returnPoint = approachArray[approachIndex];
  approachIndex += approachDir;

  return returnPoint;
}

Point getHomePoint() {
  return homePoint;
}
/*  Checks if the arms have completed the approach array (and the block is attached to the chassis). 
 *  Returns true if the approach array has been exhausted, false otherwise.
 */
bool finishedApproaching() {
  return approachIndex == approachLength;
}

/*  Checks if the arms have completed returning from the approach array.
 *  Returns true if the approach array has been exhausted, false otherwise.
 */
bool finishedReturning() {
  return approachIndex < 0;
}

/*  Returns the staging point. */
Point getStaging() {
  return staging;
}

/*  Returns a point directly above the block-catcher. */
Point getBlockCatchUp() {
  return blockCatchUp;
}

/*  Returns the position to grab the block from the block-catcher. */
Point getBlockCatchDown() {
  return blockCatchDown;
}

/*  Returns true if the block is on the far side of the chassis. */
bool isFarSide(int pos) {
  return 
    pos == 3 || 
    pos == 4 || 
    pos == 5 || 
    pos == 14 || 
    pos == 15 || 
    pos == 16;
}

/*  Returns true if the block is on the left side of the chassis. */
bool isLeftSide(int pos) {
  return 
    pos == 0 || 
    pos == 1 || 
    pos == 2 || 
    pos == 12 || 
    pos == 13 ||
    pos == 14;
}

/*  Returns true if the block is on the right side of the chassis. */
bool isRightSide(int pos) {
  return 
    pos == 6 || 
    pos == 7 || 
    pos == 8 || 
    pos == 16 ||
    pos == 17 || 
    pos == 18;
}

/*  Returns true if the block is on the close side of the chassis. */
bool isCloseSide(int pos) {
  return 
    pos == 9 || 
    pos == 10 || 
    pos == 11 ||
    pos == 12 ||
    pos == 18 || 
    pos == 19;
}

/*  Returns true if the block is on the top side of the chassis. */
bool isOnTop(int pos) {
  return pos > 11;
}

/*  Returns true if the block is being sent to the discard pile. */
bool isDiscard(int pos) {
  return pos == -1;
}

void discardApproach() {
  approachLength = 3;

  approachArray[0] = homePoint;
  approachArray[1] = discardUp;
  approachArray[2] = discardDown;
}

void topSideApproach(int pos) {
  float endX;
  float endY;
  float endZ;

  endZ = chassisCenter.z + 2.0;

  // Get lateral placement position and move to side
  if (isLeftSide(pos)) {
    endX = chassisCenter.x + 3.0;
  } else if (isRightSide(pos)) {
    endX = chassisCenter.x - 3.0;
  } else {
    endX = chassisCenter.x;
  }
  if (isFarSide(pos)) {
    endY = chassisCenter.y + 3.0;
  } else if (isCloseSide(pos)) {
    endY = chassisCenter.y - 3.0;
  } else {
    endY = chassisCenter.y;
  }

  if (isLeftSide(pos)) {
    approachArray[1] = {endX + 3.0, staging.y, staging.z};
    approachArray[2] = {endX + 3.0, endY, staging.z};
    approachArray[3] = {endX + 1.0, endY, staging.z};
    approachArray[4] = {endX, endY, staging.z};
    approachArray[5] = {endX, endY, endZ};
  }
  else if (isRightSide(pos)) {
    approachArray[1] = {endX - 3.0, staging.y, staging.z};
    approachArray[2] = {endX - 3.0, endY, staging.z};
    approachArray[3] = {endX - 1.0, endY, staging.z};
    approachArray[4] = {endX, endY, staging.z};
    approachArray[5] = {endX, endY, endZ};
  }
  else if (isFarSide(pos)) {
    // Takes a wide swing around the right side of the chassis
    approachArray[1] = {chassisCenter.x - 6.0, staging.y, staging.z};
    approachArray[2] = {chassisCenter.x - 6.0, endY + 3.0, staging.z};

    // Settles into its desired x position before dropping into desired z position
    approachArray[3] = {endX, endY + 1.0, staging.z};
    approachArray[4] = {endX, endY, endZ + 1.0};
    approachArray[5] = {endX, endY, endZ};
  }
  else if (isCloseSide(pos)) {
    // Settles into close staging position
    approachArray[1] = {chassisCenter.x, staging.y, staging.z};
    approachArray[2] = {endX, staging.y, staging.z};

    // Walks into final placement position
    approachArray[3] = {endX, endY - 1.0, staging.z};
    approachArray[4] = {endX, endY, staging.z};
    approachArray[5] = {endX, endY, endZ};
  }

  approachLength = 6;
}

void farSideApproach(int pos) {
  float endX;
  float endY;
  float endZ;

  endZ = chassisCenter.z;

  // Wall is 4" from center + 1" for block size
  endY = chassisCenter.y + 4.0 + 1.0;

  if (pos == 3) {
    endX = chassisCenter.x + 3.0;
  } else if(pos == 5) {
    endX = chassisCenter.x - 3.0;
  } else {
    endX = chassisCenter.x;
  }

  // Takes a wide swing around the right side of the chassis
  approachArray[1] = {chassisCenter.x - 6.0, staging.y, staging.z};
  approachArray[2] = {chassisCenter.x - 6.0, chassisCenter.y, staging.z};
  approachArray[3] = {chassisCenter.x - 5.0, endY + 1.0, staging.z};

  // Settles into its desired x position before dropping into desired z position
  approachArray[4] = {chassisCenter.x, endY + 0.25, staging.z};
  approachArray[5] = {endX, endY + 0.25, staging.z};
  approachArray[6] = {endX, endY + 0.25, endZ};

  // Walks into final placement position
  approachArray[7] = {endX, endY, endZ};

  approachLength = 8;
}

void leftSideApproach(int pos) {
  float endX;
  float endY;
  float endZ;

  endZ = chassisCenter.z;
  endX = chassisCenter.x + 4.0 + 1.0;

  if (pos == 2) {
    endY = chassisCenter.y + 3.0;
  } else if (pos == 0) {
    endY = chassisCenter.y - 3.0;
  } else {
    endY = chassisCenter.y;
  }

  // Swings left to avoid close-block collisions
  approachArray[1] = {endX + 3.0, staging.y, staging.z};

  // Moves up into side-staging position
  approachArray[2] = {endX + 3.0, chassisCenter.y, endZ};

  // Walks into final placement location
  approachArray[3] = {endX + 3.0, endY, endZ};
  approachArray[4] = {endX + 1.0, endY, endZ};
  approachArray[5] = {endX, endY, endZ};

  approachLength = 6;
}

void rightSideApproach(int pos) {
  float endX;
  float endY;
  float endZ;

  endZ = chassisCenter.z;
  endX = chassisCenter.x - 4.0 - 1.0;

  if (pos == 6) {
    endY = chassisCenter.y + 3.0;
  } else if (pos == 8) {
    endY = chassisCenter.y - 3.0;
  } else {
    endY = chassisCenter.y;
  }

  // Swings right to avoid close-block collisions
  approachArray[1] = {endX - 3.0, staging.y, staging.z};

  // Moves up into side-staging position
  approachArray[2] = {endX - 3.0, chassisCenter.y, endZ};

  // Walks into final placement location
  approachArray[3] = {endX - 3.0, endY, endZ};
  approachArray[4] = {endX - 1.0, endY, endZ};
  approachArray[5] = {endX, endY, endZ};

  approachLength = 6;
}

void closeSideApproach(int pos) {
  float endX;
  float endY;
  float endZ;

  endZ = chassisCenter.z;

  // Wall is 4" from center + 1" for block size
  endY = chassisCenter.y - 4.0 - 1.0;

  if (pos == 11) {
    endX = chassisCenter.x + 3.0;
  } else if(pos == 9) {
    endX = chassisCenter.x - 3.0;
  } else {
    endX = chassisCenter.x;
  }

  // Settles into close staging position
  approachArray[1] = {chassisCenter.x, staging.y, staging.z};
  approachArray[2] = {endX, staging.y, staging.z};

  // Walks into final placement position
  approachArray[3] = {endX, endY - 1.0, staging.z};
  approachArray[4] = {endX, endY - 1.0, endZ};
  approachArray[5] = {endX, endY, endZ};

  approachLength = 6;
}