// Pins
#define IN1 9
#define IN2 10
#define IN3 11
#define IN4 12

#define CENA 45
#define CIN1 47
#define CIN2 49

#define ENCL1 18
#define ENCL2 19
#define ENCR1 20
#define ENCR2 21

// Motor variables - units in degrees
// Speeds from 0.0 to 1.0 (percentage of max PWM speed)
float maxVoltage;
float avgSpeedDes;
float leftVoltage;
float rightVoltage;

float leftSpeed;
float leftSpeedDes;
float leftSpeedOld;
float rightSpeed;
float rightSpeedDes;
float rightSpeedOld;

float leftDist;
float leftDistOld;
float leftDistDes;
float leftIntegralError;
float leftDistFinal;

float rightDist;
float rightDistOld;
float rightDistDes;
float rightIntegralError;
float rightDistFinal;

// Encoders
Encoder leftEncoder(ENCL2, ENCL1);
Encoder rightEncoder(ENCR1, ENCR2);
long leftCount;
long rightCount;

// Pin output values
int DC1Speed;
int DC2Speed;
int DC3Speed;
int DC4Speed;

// Closed-loop control
float KpLeft;
float KiLeft;
float KdLeft;
float KpRight;
float KiRight;
float KdRight;

// Wheels
float gearRatio = 100;
long countsPerRevolution = 64*gearRatio;

float revPerCount = 1.0/float(countsPerRevolution);
float radPerCount = 2.0*PI*revPerCount;
float degPerCount = 360.0*revPerCount;

float wheelRadius = 4.2/2.54; // inches
float inPerCount = radPerCount*wheelRadius;
float degPerIn = 180.0/wheelRadius/PI;
float inPerDeg = 1.0/degPerIn;
float wheelWidth = 9.2;
float bodyWheelRatio = wheelWidth*0.5/wheelRadius;

/*  Sets up the DC motors that drive the wheels. Assigns the motors to the correct pins and prepares them for
 *  operation.
 */
void motorSetup() {
  // Pin setup
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(CENA, OUTPUT);
  pinMode(CIN1, OUTPUT);
  pinMode(CIN2, OUTPUT);

  // Constrain motors to 80% speed;
  maxVoltage = 8;

  // Set proportionality
  KpLeft = 0;
  KiLeft = 0;
  KdLeft = 0;
  KpRight = 0;
  KiRight = 0;
  KdRight = 0;

  // Set all values to 0
  resetMotors();
}

/*  Wheel driving loop function. Updates position and velocity via encoder, then calculates new velocity
 *  based on closed-loop control for the desired drive trajectory.
 *  Call every loop to re-calculate drive characteristics for closed-loop control while dead-reckoning.
 *  Returns true while the wheels have distance to cover, and false when they have reached their desired angle.
 */
void wheelDrive() {
  // Update wheel position and velocity
  leftCount = leftEncoder.read();
  rightCount = rightEncoder.read();

  // Convert counts to inches
  leftDist = leftCount*inPerCount;
  rightDist = rightCount*inPerCount;

  leftSpeed = (leftDist - leftDistOld)/dt;
  rightSpeed = (rightDist - rightDistOld)/dt;

  // Get new desired position
  if (!(abs(leftDistDes) >= abs(leftDistFinal))) {
    leftDistDes += leftSpeedDes*dt;
  }
  if (!(abs(rightDistDes) >= abs(rightDistFinal))) {
    rightDistDes += rightSpeedDes*dt;
  }

  float leftError = (leftDistDes - leftDist);
  float rightError = (rightDistDes - rightDist);

  float leftSpeedError = (leftSpeedDes - leftSpeed);
  float rightSpeedError = (rightSpeedDes - rightSpeed);

  leftIntegralError += leftError*dt;
  rightIntegralError += rightError*dt;

  // Get desired wheel speeds from PID controller
  leftVoltage = KpLeft*leftError + KiLeft*leftIntegralError + KdLeft*leftSpeedError;
  rightVoltage = KpRight*rightError + KiRight*rightIntegralError + KdRight*rightSpeedError;

  // Constrain speeds to within maximum speed range
  constrainSpeeds();

  // Get and output necessary drive voltage
  getPinSpeeds();
  writePinSpeeds();

  // Cache old angles for speed calculation
  leftDistOld = leftDist;
  rightDistOld = rightDist;
}

/*  Drives the wheels based on the provided wheel speeds, given from
 *  0.0 to 1.0 as a percentage of max PWM speed 
 */
void wheelDriveSpeed(float leftSpeed, float rightSpeed) {
  leftVoltage = leftSpeed;
  rightVoltage = rightSpeed;

  constrainSpeeds();

  // if (debugMode) {
  //   debugPrintln("Speeds: ");
  //   debugPrint(leftVoltage); debugPrint(" ");
  //   debugPrintln(rightVoltage);
  // }

  getPinSpeeds();

  analogWrite(IN1, DC1Speed);
  analogWrite(IN2, DC2Speed);
  analogWrite(IN3, DC3Speed);
  analogWrite(IN4, DC4Speed);
}

/*  Brakes the DC Motors controlling the wheels. */
void wheelBrake() {
  // Set all pins to max for magnetic braking
  DC1Speed = 255;
  DC2Speed = 255;
  DC3Speed = 255;
  DC4Speed = 255;

  writePinSpeeds();

  if (debugMode) {
    forcedPrintln("Braking.");
  }
}

/*  Drive straight for the given distance (in) at the given speed (in/s).
 *  use negative directions for reverse movement.
 */
void driveStraight(float dist, float speed) {
  int direction = (dist > 0) - (dist < 0);

  // Reset motors
  resetMotors();

  avgSpeedDes = abs(speed);
  leftSpeedDes = avgSpeedDes;
  rightSpeedDes = avgSpeedDes;
  leftDistFinal = dist;
  rightDistFinal = dist;
}

/*  Rotate in place a given angular distance (deg) at a given speed (deg/s).
 *  Positive values are counter-clockwise and negative values are clockwise.
 */
void rotate(float dist, float speed) {
  int direction = (dist > 0) - (dist < 0);

  // Reset encoders
  leftEncoder.write(0);
  rightEncoder.write(0);
  leftCount = 0;
  rightCount = 0;

  avgSpeedDes = direction*abs(speed)*bodyWheelRatio*PI/180.0;

  // If avgSpeed > 0, leftSpeed should be negative for CCW movement
  leftSpeedDes = -avgSpeedDes;
  rightSpeedDes = avgSpeedDes;

  leftDistFinal = -dist*PI/180.0*bodyWheelRatio;
  rightDistFinal = -rightDistFinal;
}

/*  Turn in a circular arc of the given inner-wheel radius (in) with the given average wheel speed (in/s).
 *  Positive values are counter-clockwise and negative values are clockwise.
 */
void turn(float radius, float speed) {
  float turnRadius = abs(radius);

  float bodyTurnRatio = (wheelWidth + turnRadius)/turnRadius;
  float wheelTurnRatio = turnRadius/wheelRadius;

  // Reset encoders
  leftEncoder.write(0);
  rightEncoder.write(0);
  leftCount = 0;
  rightCount = 0;

  // Convert speed from in/s to deg/s
  avgSpeedDes = speed*degPerIn;

  if (radius > 0.0) {
    leftSpeedDes = 2.0*avgSpeedDes/(1.0 + bodyTurnRatio);
    rightSpeedDes = avgSpeedDes*bodyTurnRatio;

    leftDistFinal = 180.0*wheelTurnRatio;
    rightDistFinal = 180.0*bodyTurnRatio*wheelTurnRatio;
  } else {
    rightSpeedDes = 2.0*avgSpeedDes/(1.0 + bodyTurnRatio);
    leftSpeedDes = avgSpeedDes*bodyTurnRatio;

    rightDistFinal = 180.0*wheelTurnRatio;
    leftDistFinal = 180.0*bodyTurnRatio*wheelTurnRatio;
  }
}

/*  Converts the desired wheel speeds (0.0-1.0) into analog pin speeds (0-255).
 */
void getPinSpeeds() {
  // Checks wheel direction and adjusts which pins are high and low accordingly
  int leftDir = 1;
  int rightDir = 1;

  if (leftVoltage < 0) leftDir = 0;
  if (rightVoltage < 0) rightDir = 0;              

  // Convert PWM ratio to analogWrite
  int leftWrite = leftVoltage*255*.1;
  int rightWrite = rightVoltage*255*.1;

  // Set pin speeds to analogWrite values
  DC2Speed = (1-leftDir)*leftWrite;
  DC1Speed = leftDir*leftWrite;
  DC4Speed = rightDir*rightWrite;
  DC3Speed = (1-rightDir)*rightWrite;

  // if (debugMode) {
  //   debugPrintln("Pin speeds:");
  //   debugPrintln(DC1Speed);
  //   debugPrintln(DC2Speed);
  //   debugPrintln(DC3Speed);
  //   debugPrintln(DC4Speed);
  // }
}

/*  Writes the defined analog pin speeds to the defined analog pins. */
void writePinSpeeds() {
  analogWrite(IN1, DC1Speed);
  analogWrite(IN2, DC2Speed);
  analogWrite(IN3, DC3Speed);
  analogWrite(IN4, DC4Speed);

}

/*  Constrains the left and right output values to be within the defined maximum speed range set by
 *  +- maxVoltage. Both values are scaled appropriately to maintain speed ratio, with signs left unchanged.
 */
void constrainSpeeds() {
  if (leftVoltage > maxVoltage) {
    leftVoltage = maxVoltage;
    rightVoltage *= maxVoltage/leftVoltage;
  } else if (-leftVoltage > maxVoltage) {
    leftVoltage = -maxVoltage;
    rightVoltage *= -maxVoltage/leftVoltage;
  }
  if (rightVoltage > maxVoltage) {
    rightVoltage = maxVoltage;
    leftVoltage *= maxVoltage/rightVoltage;
  } else if (-rightVoltage > maxVoltage) {
    rightVoltage = -maxVoltage;
    leftVoltage *= -maxVoltage/rightVoltage;
  }
}

/*  Checks if the wheels have reached their final calculated position. */
bool atDestination() {
  return (abs(leftDistDes) >= abs(leftDistFinal)) && (abs(rightDistDes) >= abs(rightDistFinal));
}

/*  Resets motor control configuration for future controls. Erases all desired angles and speeds and set
 *  encoders back to 0.
 */
void resetMotors() {
  leftEncoder.write(0);
  rightEncoder.write(0);
  leftCount = 0;
  rightCount = 0;

  leftVoltage = 0;
  rightVoltage = 0;

  leftSpeed = 0;
  leftSpeedDes = 0;
  leftSpeedOld = 0;
  rightSpeed = 0;
  rightSpeedDes = 0;
  rightSpeedOld = 0;

  leftDist = 0;
  leftDistOld = 0;
  leftDistDes = 0;
  leftIntegralError = 0;
  leftDistFinal = 0;

  rightDist = 0;
  rightDistOld = 0;
  rightDistDes = 0;
  rightIntegralError = 0;
  rightDistFinal = 0;
}

/*  Begins tracking the total traveled distance. Used for non-PID control travel, like line-following. */
void trackDistance(float dist) {
  // Reset encoders
  leftEncoder.write(0);
  rightEncoder.write(0);

  // Reset distance data
  leftDist = 0.0;
  rightDist = 0.0;
}

float getTrackDistance() {
  leftCount = leftEncoder.read();
  rightCount = rightEncoder.read();

  // Convert count to inches
  leftDist = leftCount*inPerCount;
  rightDist = rightCount*inPerCount;

  // Return average of both wheels
  return 0.5*(leftDist + rightDist);
}