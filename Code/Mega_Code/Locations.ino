struct Point blockCatchUp = {6, 0, 4};
struct Point blockCatchDown = {6, 0, 0};
struct Point staging = {-8, -10, 4};
struct Point homePoint = staging;

const int approachLength = 4;
struct Point approachArray[approachLength];
int approachIndex = 0;
int approachDir = 1;

/*  Sets up pre-calculated positions for the robot to use. */
void loadPoint1() {
  approachArray[0] = staging;
  approachArray[1] = {-1, -15, 4};
  approachArray[2] = {-1, -15, 0};
  approachArray[3] = {1.75, -15, 0};
}

/*  Sets up pre-calculated positions for the robot to use. */
void loadPoint2() {
  approachArray[0] = staging;
  approachArray[1] = {3.5, -10, 4};
  approachArray[2] = {3.5, -15, 4};
  approachArray[3] = {3.5, -15, 3};
}

/*  Sets the approach array to the beginning of the array and generates
 *  the approach points based on the position. 
 */
void approachPosition(int position) {
  approachIndex = 0;
  approachDir = 1;
}

/*  Utilizes the already-generated approach array to return to the staging position,
 *  moving the arms out of the way.
 */
void approachReturn() {
  approachDir = -1;
  approachIndex = approachLength - 1;
}

/*  Gets the next point of form (x,y,z) of the approach array, based on whether the arm
 *  is approaching the placement point or the staging location.
 *  Returns an empty point if the array index is out of bounds. 
 */
struct Point getNextPoint() {
  if (approachIndex >= approachLength) {
    return homePoint;
  } else if (approachIndex < 0) {
    return homePoint;
  }

  struct Point returnPoint = approachArray[approachIndex];
  approachIndex += approachDir;

  return returnPoint;
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
struct Point getStaging() {
  return staging;
}

struct Point getBlockCatchUp() {
  return blockCatchUp;
}

struct Point getBlockCatchDown() {
  return blockCatchDown;
}