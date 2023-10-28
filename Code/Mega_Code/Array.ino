// PINS
#define LS1 34
#define LS2 46
#define LS3 32
#define LS4 44
#define LS5 30
#define LS6 42
#define LS7 28
#define LS8 40
#define LS9 26
#define LS10 38
#define LS11 24
#define LS12 36
#define LS13 22
#define LSIN 48

// Sensors
QTRSensors qtr;
#define SENSORCOUNT 13
uint16_t sensorValues[SENSORCOUNT];

// Calibration Data
uint16_t baseValues[SENSORCOUNT]{316, 228, 228, 228, 228, 228, 228, 228, 228, 228, 228, 228, 228};
uint16_t maxValues[SENSORCOUNT]{1632, 1228, 1384, 1460, 1220, 1452, 1628, 1628, 1712, 1544, 1624, 1456, 1456};

// Proportionality Constant
float LFKp;

/*  Sets up the line-following array, intializing the sensor to the proper pins and
 *  adjusting the calibration values.
 */
void arraySetup() {
  qtr.setTypeRC();
  qtr.setSensorPins((const uint8_t[]){LS1, LS2, LS3, LS4, LS5, LS6, LS7, LS8, LS9, LS10, LS11, LS12, LS13}, SENSORCOUNT);
  qtr.setEmitterPin(LSIN);

  LFKp = 0.05;
}

/*  Sets the forward wheel speed based on the provided desired speed and the
 *  line-following error with the proportionality constant defined above.
 */
void lineFollow(float avgSpd) {
  float leftWheelSpeed = avgSpd;
  float rightWheelSpeed = avgSpd;

  // Adjust wheel speeds based on error-term
  float error = getLineError();
  leftWheelSpeed += LFKp*error;
  rightWheelSpeed -= LFKp*error;

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
float getLineError() {
  // Nominal line position
  float d0 = 7.0;

  qtr.read(sensorValues);

  if (debugMode) {
    debugPrintln("Sensor Values:");
  }
  // Map the sensor reading to their calibrated range
  for (int i = 0; i < SENSORCOUNT; i++) {
    if (sensorValues[i] < baseValues[i]) {
      sensorValues[i] = baseValues[i];
    } else if (sensorValues[i] > maxValues[i]) {
      sensorValues[i] = maxValues[i];
    }

    sensorValues[i] = map(sensorValues[i], baseValues[i], maxValues[i], 0, 2550);

    if (debugMode) {
      int val = sensorValues[i];
      debugPrint(val);
      debugPrint(" ");
    }
  }

  // Calculate which sensor the line is centered beneath
  long readingSum = 0;
  long weightedSum = 0;

  for (int i = 0; i < SENSORCOUNT; i++) {
    readingSum += sensorValues[i];
    weightedSum += sensorValues[i]*long(i+1);
  }

  float location = (float)weightedSum / (float)readingSum;

  if (debugMode) {
    debugPrintln("Current line location:");
    debugPrintln(location);
    debugPrintln("");
  }

  // return difference from nominal
  return d0 - location;
}