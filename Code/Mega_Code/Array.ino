// PINS
#define RL1 34
#define RL2 46
#define RL3 32
#define RL4 44
#define RL5 30
#define RL6 42
#define RL7 28
#define RL8 40
#define RL9 26
#define RL10 38
#define RL11 24
#define RL12 36
#define RL13 22
#define RIN 48

#define FL1 A1
#define FL2 A2
#define FL3 A3
#define FL4 A4
#define FL5 A5
#define FL6 A6
#define FL7 A7
#define FL8 A8
#define FIN 53

// Sensors
QTRSensors front, rear;
#define REARCOUNT 13
#define FRONTCOUNT 8
uint16_t rearValues[REARCOUNT];
uint16_t frontValues[FRONTCOUNT];

// Calibration Data
uint16_t rearBaseValues[REARCOUNT]{316, 228, 228, 228, 228, 228, 228, 228, 228, 228, 228, 228, 228};
uint16_t rearMaxValues[REARCOUNT]{1632, 1228, 1384, 1460, 1220, 1452, 1628, 1628, 1712, 1544, 1624, 1456, 1456};

uint16_t frontBaseValues[FRONTCOUNT]{316, 228, 228, 228, 228, 228, 228, 228};
uint16_t frontMaxValues[FRONTCOUNT]{1632, 1228, 1384, 1460, 1220, 1452, 1628, 1628};

// Proportionality Constants
float RKp, FKp;

// Line thickness for intersection checking
int lineThickness = 4;

/*  Sets up the line-following array, intializing the sensor to the proper pins and
 *  adjusting the calibration values.
 */
void arraySetup() {
  rear.setTypeRC();
  rear.setSensorPins((const uint8_t[]){RL1, RL2, RL3, RL4, RL5, RL6, RL7, RL8, RL9, RL10, RL11, RL12, RL13}, REARCOUNT);
  rear.setEmitterPin(RIN);

  front.setTypeRC();
  front.setSensorPins((const uint8_t[]){FL1, FL2, FL3, FL4, FL5, FL6, FL7, FL8}, FRONTCOUNT);
  front.setEmitterPin(FIN);

  RKp = 0.05;
  FKp = 0.05;
}

/*  Sets the forward wheel speed based on the provided desired speed and the
 *  line-following error with the proportionality constant defined above.
 */
void forwardFollow(float avgSpd) {
  float leftWheelSpeed = avgSpd;
  float rightWheelSpeed = avgSpd;

  // Adjust wheel speeds based on error-term
  float error = getForwardError();
  leftWheelSpeed += FKp*error;
  rightWheelSpeed -= FKp*error;

  if (debugMode) {
    debugPrintln("Adjusted Wheel Speeds:");
    debugPrint(leftWheelSpeed);
    debugPrint(" ");
    debugPrint(rightWheelSpeed);
    debugPrintln(" ");
  }

  wheelDriveSpeed(leftWheelSpeed, rightWheelSpeed);
}

/*  Sets the reverse wheel speed based on the provided desired speed and the
 *  line-following error with the proportionality constant defined above.
 */
void reverseFollow(float avgSpd) {
  float leftWheelSpeed = -avgSpd;
  float rightWheelSpeed = -avgSpd;

  // Adjust wheel speeds based on error-term
  float error = getReverseError();
  leftWheelSpeed -= RKp*error;
  rightWheelSpeed += RKp*error;

  if (debugMode) {
    debugPrintln("Adjusted Wheel Speeds:");
    debugPrint(leftWheelSpeed);
    debugPrint(" ");
    debugPrint(rightWheelSpeed);
    debugPrintln(" ");
  }

  wheelDriveSpeed(leftWheelSpeed, rightWheelSpeed);
}

/*  Gets the line location error against its nominal position (centered).
 */
float getForwardError() {
  // Nominal line position
  float d0 = 4;

  front.read(frontValues);

  if (debugMode) {
    debugPrintln("Sensor Values:");
  }

  mapFrontReadings();

  // Calculate which sensor the line is centered beneath
  float location = getFrontLineLocation();

  // return difference from nominal
  return d0 - location;
}

/*  Gets the line location error against its nominal position (centered).
 */
float getReverseError() {
  // Nominal line position
  float d0 = 7;

  rear.read(rearValues);

  if (debugMode) {
    debugPrintln("Sensor Values:");
  }

  mapRearReadings();

  // Calculate which sensor the line is centered beneath
  float location = getRearLineLocation();

  // return difference from nominal
  return d0 - location;
}

/*  Maps the front sensor readings to their full input range based on calibrated white and black readings. */
void mapFrontReadings() {

  // Constrain values to min and max of range
  for (int i = 0; i < FRONTCOUNT; i++) {
    if (frontValues[i] < frontBaseValues[i]) {
      frontValues[i] = frontBaseValues[i];
    } else if (frontValues[i] > frontMaxValues[i]) {
      frontValues[i] = frontMaxValues[i];
    }

    frontValues[i] = map(frontValues[i], frontBaseValues[i], frontMaxValues[i], 0, 2550);

    if (debugMode) {
      int val = frontValues[i];
      debugPrint(val);
      debugPrint(" ");
    }
  }
}

/*  Maps the rear sensor readings to their full input range based on calibrated white and black readings. */
void mapRearReadings() {

  // Constrain values to min and max of range
  for (int i = 0; i < REARCOUNT; i++) {
    if (rearValues[i] < rearBaseValues[i]) {
      rearValues[i] = rearBaseValues[i];
    } else if (rearValues[i] > rearMaxValues[i]) {
      rearValues[i] = rearMaxValues[i];
    }

    rearValues[i] = map(rearValues[i], rearBaseValues[i], rearMaxValues[i], 0, 2550);

    if (debugMode) {
      int val = rearValues[i];
      debugPrint(val);
      debugPrint(" ");
    }
  }
}

/*  Gets the specific sensor that the front line is centered below using weighted averages for sensor readings. */
float getFrontLineLocation() {
  long readingSum = 0;
  long weightedSum = 0;

  for (int i = 0; i < FRONTCOUNT; i++) {
    readingSum += frontValues[i];
    weightedSum += frontValues[i]*long(i+1);
  }

  float location = (float)weightedSum / (float)readingSum;

  if (debugMode) {
    debugPrintln("Current line location:");
    debugPrintln(location);
    debugPrintln("");
  }

  return location;
}

/*  Gets the specific sensor that the rear line is centered below using weighted averages for sensor readings. */
float getRearLineLocation() {
  long readingSum = 0;
  long weightedSum = 0;

  for (int i = 0; i < REARCOUNT; i++) {
    readingSum += rearValues[i];
    weightedSum += rearValues[i]*long(i+1);
  }

  float location = (float)weightedSum / (float)readingSum;

  if (debugMode) {
    debugPrintln("Current line location:");
    debugPrintln(location);
    debugPrintln("");
  }

  return location;
}

/*  Checks if the front line-sensor has detected a three-way or four-way intersection.
 *  Returns true if there is an intersection, or false otherwise.
 */
bool atIntersectionFront() {
  int triggerCount = 0;

  // Count the number of flipped sensors
  for (int i = 0; i < FRONTCOUNT; i++) {
    if (frontValues[i] > 1500) {
      triggerCount++;
    }
  }

  if (triggerCount > lineThickness) {
    return true;
  }

  return false;
}

/*  Checks if the rear line-sensor has detected a three-way or four-way intersection.
 *  Returns true if there is an intersection, or false otherwise.
 */
bool atIntersectionRear() {
  int triggerCount = 0;

  // Count the number of flipped sensors
  for (int i = 0; i < REARCOUNT; i++) {
    if (rearValues[i] > 1500) {
      triggerCount++;
    }
  }

  if (triggerCount > lineThickness) {
    return true;
  }

  return false;
}