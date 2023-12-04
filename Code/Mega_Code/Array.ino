// PINS
#define RL1 47
#define RL2 45
#define RL3 43
#define RL4 41
#define RL5 39
#define RL6 37
#define RL7 35
#define RL8 33
#define RL9 31
#define RL10 29
#define RL11 27
#define RL12 25
#define RL13 23
#define RIN 49

#define FL1 A15
#define FL2 A14
#define FL3 A13
#define FL4 A12
#define FL5 A11
#define FL6 A10
#define FL7 A9
#define FL8 A8
#define FIN A7

// Sensors
QTRSensors front, rear;
#define REARCOUNT 13
#define FRONTCOUNT 8
uint16_t rearValues[REARCOUNT];
uint16_t frontValues[FRONTCOUNT];

// Calibration Data
uint16_t rearBaseValues[REARCOUNT]{220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220};
uint16_t rearMaxValues[REARCOUNT]{2140, 1484, 1564, 1556, 1404, 1564, 1804, 1804, 1968, 1644, 1564, 1484, 1804};

uint16_t frontBaseValues[FRONTCOUNT]{140, 140, 92, 140, 92, 140, 92, 140};
uint16_t frontMaxValues[FRONTCOUNT]{2340, 2084, 1416, 1936, 1700, 2088, 1748, 1888};

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

  RKp = 0.12;
  FKp = 0.12;
}

/*  Sets the forward wheel speed based on the provided desired speed and the
 *  line-following error with the proportionality constant defined above.
 */
void forwardFollow(float avgSpd) {
  float leftWheelSpeed = avgSpd;
  float rightWheelSpeed = avgSpd;

  // Adjust wheel speeds based on error-term
  float error = getForwardError();
  leftWheelSpeed -= FKp*error;
  rightWheelSpeed += FKp*error;

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
  leftWheelSpeed += RKp*error;
  rightWheelSpeed -= RKp*error;
  
  wheelDriveSpeed(leftWheelSpeed, rightWheelSpeed);
}

/*  Gets the line location error against its nominal position (centered).
 */
float getForwardError() {
  // Nominal line position
  float d0 = 4;

  front.read(frontValues);

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
    frontValues[i] = constrain(frontValues[i], frontBaseValues[i], frontMaxValues[i]);

    frontValues[i] = map(frontValues[i], frontBaseValues[i], frontMaxValues[i], 0, 2550);
  }
}

/*  Maps the rear sensor readings to their full input range based on calibrated white and black readings. */
void mapRearReadings() {

  // Constrain values to min and max of range
  for (int i = 0; i < REARCOUNT; i++) {
    rearValues[i] = constrain(rearValues[i], rearBaseValues[i], rearMaxValues[i]);

    rearValues[i] = map(rearValues[i], rearBaseValues[i], rearMaxValues[i], 0, 2550);
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

  if (triggerCount > 2) {
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

  if (triggerCount > 3) {
    return true;
  }

  return false;
}