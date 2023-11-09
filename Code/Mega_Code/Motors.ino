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
float maxMotorSpeed;
float avgSpeedDes;
float leftOutput;
float rightOutput;

float leftSpeed;
float leftSpeedDes;
float rightSpeed;
float rightSpeedDes;

float leftAngle;
float leftAngleOld;
float leftAngleDes;
float leftAngleFinal;

float rightAngle;
float rightAngleOld;
float rightAngleDes;
float rightAngleFinal;

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
float Kp;

// Wheels
float gearRatio = 70;
long countsPerRevolution = 64*gearRatio;

float revPerCount = 1.0/float(countsPerRevolution);
float degPerCount = 360.0*revPerCount;

float wheelRadius = 4.2/2.54;
float degPerIn = 180.0/wheelRadius/PI;
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

  // Constrain motors to 50% speed;
  maxMotorSpeed = 0.5;

  // Set proportionality
  Kp = 0.02;

  // Set all values to 0
  resetMotors();
}

/*  Wheel driving loop function. Updates position and velocity via encoder, then calculates new velocity
 *  based on closed-loop control for the desired drive trajectory.
 *  Call every loop to re-calculate drive characteristics for closed-loop control while dead-reckoning.
 *  Returns true while the wheels have distance to cover, and false when they have reached their desired angle.
 */
void wheelDrive() {

  // if (debugMode) {
  //   debugPrintln("Wheel Angles: ");
  //   debugPrint(leftAngle); debugPrint(" ");
  //   debugPrint(leftAngleDes); debugPrint(" ");
  //   debugPrintln(leftAngleFinal);
  //   debugPrint(rightAngle); debugPrint(" ");
  //   debugPrint(rightAngleDes); debugPrint(" ");
  //   debugPrintln(rightAngleFinal);
  // }

  // Update wheel position and velocity
  leftCount = leftEncoder.read();
  rightCount = rightEncoder.read();
  
  // if (debugMode) {
  //   debugPrint("Counts: ");
  //   debugPrint(leftCount); debugPrint(" "); debugPrintln(rightCount);
  // }

  // Convert encoder counts to angles in degrees
  leftAngle = leftCount*degPerCount;
  rightAngle = rightCount*degPerCount;


  // Calculate angular speed in degrees/second
  // leftSpeed = (leftAngle - leftAngleOld)/dt;
  // rightSpeed = (rightAngle - rightAngleOld)/dt;

  // Get new desired position
  if (!(abs(leftAngleDes) >= abs(leftAngleFinal))) {
    debugPrintln("left");
    leftAngleDes += leftSpeedDes*dt/1000.;
  }
  if (!(abs(rightAngleDes) >= abs(rightAngleFinal))) {
    rightAngleDes += rightSpeedDes*dt/1000.;
    debugPrintln("right");
  }

  // Get desired wheel speeds from closed-loop equation
  leftOutput = Kp*(leftAngleDes - leftAngle);
  rightOutput = Kp*(rightAngleDes - rightAngle);

  if (debugMode) {
    debugPrintln("OUTPUT SPEEDS: ");
    debugPrint(leftOutput); debugPrint("  "); debugPrint(rightOutput);
  }

  // Constrain speeds to within maximum speed range
  constrainSpeeds();

  // Get and output necessary drive voltage
  getPinSpeeds();
  writePinSpeeds();

  // Cache old angles for speed calculation
  leftAngleOld = leftAngle;
  rightAngleOld = rightAngle;
}

/*  Drives the wheels based on the provided wheel speeds, given from
 *  0.0 to 1.0 as a percentage of max PWM speed 
 */
void wheelDriveSpeed(float leftSpeed, float rightSpeed) {
  leftOutput = leftSpeed;
  rightOutput = rightSpeed;

  constrainSpeeds();

  // if (debugMode) {
  //   debugPrintln("Speeds: ");
  //   debugPrint(leftOutput); debugPrint(" ");
  //   debugPrintln(rightOutput);
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

  // Reset encoders
  leftEncoder.write(0);
  rightEncoder.write(0);
  leftCount = 0;
  rightCount = 0;

  // Convert speed from in/s to deg/s
  avgSpeedDes = direction*abs(speed)*degPerIn;

  if (debugMode) {
    forcedPrintln("Average desired speed: ");
    forcedPrintln(avgSpeedDes);
  }

  leftSpeedDes = avgSpeedDes;
  rightSpeedDes = avgSpeedDes;

  // Convert distances from in to deg
  leftAngleFinal = dist*degPerIn;
  rightAngleFinal = leftAngleFinal;

  if (debugMode) {
    forcedPrintln("Final angles: ");
    forcedPrintln(leftAngleFinal);
    forcedPrintln(rightAngleFinal);
    forcedPrintln(leftSpeedDes);
    forcedPrintln(rightSpeedDes);
  }
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

  avgSpeedDes = direction*abs(speed)*bodyWheelRatio;

  if (debugMode) {
    forcedPrintln("Average desired speed: ");
    forcedPrintln(avgSpeedDes);
  }

  // If avgSpeed > 0, leftSpeed should be negative for CCW movement
  leftSpeedDes = -avgSpeedDes;
  rightSpeedDes = avgSpeedDes;

  leftAngleFinal = -dist*bodyWheelRatio;
  rightAngleFinal = -leftAngleFinal;

  if (debugMode) {
    forcedPrintln("Final angles: ");
    forcedPrintln(leftAngleFinal);
    forcedPrintln(rightAngleFinal);
    forcedPrintln(leftSpeedDes);
    forcedPrintln(rightSpeedDes);
  }
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

    leftAngleFinal = 180.0*wheelTurnRatio;
    rightAngleFinal = 180.0*bodyTurnRatio*wheelTurnRatio;
  } else {
    rightSpeedDes = 2.0*avgSpeedDes/(1.0 + bodyTurnRatio);
    leftSpeedDes = avgSpeedDes*bodyTurnRatio;

    rightAngleFinal = 180.0*wheelTurnRatio;
    leftAngleFinal = 180.0*bodyTurnRatio*wheelTurnRatio;
  }
}

/*  Converts the desired wheel speeds (0.0-1.0) into analog pin speeds (0-255).
 */
void getPinSpeeds() {
  // Checks wheel direction and adjusts which pins are high and low accordingly
  int leftDir = 1;
  int rightDir = 1;

  if (leftOutput < 0) leftDir = 0;
  if (rightOutput < 0) rightDir = 0;              

  // Convert PWM ratio to analogWrite
  int leftWrite = leftOutput*255;
  int rightWrite = rightOutput*255;

  // Set pin speeds to analogWrite values
  DC2Speed = (1-leftDir)*leftWrite;
  DC1Speed = (leftDir)*leftWrite;
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
 *  +- maxMotorSpeed. Both values are scaled appropriately to maintain speed ratio, with signs left unchanged.
 */
void constrainSpeeds() {
  if (leftOutput > maxMotorSpeed) {
    leftOutput = maxMotorSpeed;
    rightOutput *= maxMotorSpeed/leftOutput;
  } else if (-leftOutput > maxMotorSpeed) {
    leftOutput = -maxMotorSpeed;
    rightOutput *= -maxMotorSpeed/leftOutput;
  }
  if (rightOutput > maxMotorSpeed) {
    rightOutput = maxMotorSpeed;
    leftOutput *= maxMotorSpeed/rightOutput;
  } else if (-rightOutput > maxMotorSpeed) {
    rightOutput = -maxMotorSpeed;
    leftOutput *= -maxMotorSpeed/rightOutput;
  }
}

/*  Checks if the wheels have reached their final calculated position. */
bool atDestination() {
  return (abs(leftAngleDes) >= abs(leftAngleFinal)) && (abs(rightAngleDes) > abs(rightAngleFinal));
}

/*  Resets motor control configuration for future controls. Erases all desired angles and speeds and set
 *  encoders back to 0.
 */
void resetMotors() {
  leftEncoder.write(0);
  rightEncoder.write(0);
  leftCount = 0;
  rightCount = 0;

  avgSpeedDes = 0.0;
  leftOutput = 0.0;
  rightOutput = 0.0;

  leftSpeed = 0.0;
  leftSpeedDes = 0.0;
  rightSpeed = 0.0;
  rightSpeedDes = 0.0;

  leftAngle = 0.0;
  leftAngleOld = 0.0;
  leftAngleDes = 0.0;
  leftAngleFinal = 0.0;

  rightAngle = 0.0;
  rightAngleOld = 0.0;
  rightAngleDes = 0.0;
  rightAngleFinal = 0.0;
}
