// PINS
#define LS1 22
#define LS2 24
#define LS3 26
#define LS4 28
#define LS5 30
#define LS6 32
#define LS7 34
#define LS8 36
#define LS9 38
#define LS10 40
#define LS11 42
#define LS12 44
#define LS13 46
#define LSIN 48

// Sensors
QTRSensors qtr;
#define SENSORCOUNT 13
uint16_t sensorValues[SENSORCOUNT];

// Calibration Data
uint16_t baseValues[SENSORCOUNT];

// Proportionality Constant
float kP;

/*  Sets up the line-following array, intializing the sensor to the proper pins and
 *  adjusting the calibration values.
 */
void arraySetup() {
  qtr.setTypeRC();
  qtr.setSensorPins((const uint8_t[]){LS1, LS2, LS3, LS4, LS5, LS6, LS7, LS8, LS9, LS10, LS11, LS12, LS13}, SENSORCOUNT);
  qtr.setEmitterPin(LSIN);

  kP = 0.025;
}

/*  Sets the forward wheel speed based on the provided desired speed and the
 *  line-following error with the proportionality constant defined above.
 */
void lineFollow(float avgSpd) {
  // Get default wheel speeds
  getWheelSpeeds(FORWARD, avgSpd, 0.0);

  // Adjust wheel speeds based on error-term
  float error = getLineError();
  leftWheelSpeed -= kP*error;
  rightWheelSpeed += kP*error;
}

/*  Gets the line location error against its nominal position (centered).
 */
float getLineError() {
  // Nominal line position
  float d0 = 7.0;

  qtr.read(sensorValues);

  // Adjust each reading by calibration value
  for (int i = 0; i < SENSORCOUNT; i++) {
    sensorValues[i] -= baseValues[i];
  }

  // Calculate which sensor the line is centered beneath
  long readingSum = 0;
  long weightedSum = 0;
  for (int i = 0; i < SENSORCOUNT; i++) {
    readingSum += sensorValues[i];
    weightedSum += sensorValues[i]*long(i);
  }
  float location = (float)weightedSum / (float)readingSum;

  if (debugMode) {
    debugPrintln("Current line location:");
    debugPrintln(location);
    debugPrintln("");
  }

  return d0 - location;
}