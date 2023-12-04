// Pins
#define IN1 10
#define IN2 9
#define IN3 11
#define IN4 12

#define ENCL1 20
#define ENCL2 21
#define ENCR1 19
#define ENCR2 18

// Motor variables - units in degrees
float leftVoltage;
float rightVoltage;

float leftSpeed;
float leftSpeedf;
float leftSpeedDes;
float leftSpeedOld;
float rightSpeed;
float rightSpeedf;
float rightSpeedDes;
float rightSpeedOld;

float leftDist;
float leftDistOld;
float leftDistDes;
float leftError;
float leftSpeedError;
float leftIntegralError = 0;
float leftDistFinal;

float rightDist;
float rightDistOld;
float rightDistDes;
float rightError;
float rightSpeedError;
float rightIntegralError = 0;
float rightDistFinal;

// Encoders
Encoder leftEncoder(ENCL2, ENCL1);
Encoder rightEncoder(ENCR1, ENCR2);
long leftCount = 0;
long rightCount = 0;

// Pin output values
int DC1Speed;
int DC2Speed;
int DC3Speed;
int DC4Speed;

// PID Params
float Kp;
float Kd;
float Ki;

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
float wheelWidth = 9.5;
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

  // PID Control Parameters
  Kp = 20;
  Kd = 0.4;
  Ki = 8;

  // Set all values to 0
  resetMotors();
}

/*  Wheel driving loop function. Updates position and velocity via encoder, then calculates new velocity
 *  based on closed-loop control for the desired drive trajectory.
 *  Call every loop to re-calculate drive characteristics for closed-loop control while dead-reckoning.
 *  Returns true while the wheels have distance to cover, and false when they have reached their desired angle.
 */
void PIDDrive() {

  // Encoder sensing
  leftDist = leftEncoder.read()*2*PI*revPerCount*wheelRadius;
  rightDist = rightEncoder.read()*2*PI*revPerCount*wheelRadius;

  // Compute Errors

  if (abs(rightDistDes) < abs(rightDistFinal)) {
    rightDistDes += rightSpeedDes*dt;
  }
  if (abs(leftDistDes) < abs(leftDistFinal)) {
    leftDistDes += leftSpeedDes*dt;
  }

  leftError = leftDistDes - leftDist;
  leftSpeed = (leftDist - leftDistOld)/dt; 
  leftSpeedf = .2*leftSpeed + (1 - .2)*leftSpeedf;
  leftSpeedError = leftSpeedDes - leftSpeedf; 
  leftIntegralError += leftError * dt;
  leftIntegralError = constrain(leftIntegralError, -10./Ki, 10./Ki); 

  rightError = rightDistDes - rightDist;
  rightSpeed = (rightDist-rightDistOld)/dt; 
  rightSpeedf = .2*rightSpeed + (1 - .2)*rightSpeedf;
  rightSpeedError = rightSpeedDes - rightSpeedf;
  rightIntegralError += rightError * dt;
  rightIntegralError = constrain(rightIntegralError, -10./Ki, 10./Ki); 

  // PID Control Law
  leftVoltage = Kp*leftError + Ki*leftIntegralError + Kd*leftSpeedError; // Compute voltage command
  rightVoltage = Kp*rightError + Ki*rightIntegralError + Kd*rightSpeedError;

  if (rightVoltage >= 0) {
    DC1Speed = (int)(rightVoltage *25.5);
    DC2Speed = 0;
  } else {
    DC1Speed = 0;
    DC2Speed = (int)(rightVoltage*25.5);
  }

  if (leftVoltage >= 0) {
    DC3Speed = (int)(leftVoltage*25.5);
    DC4Speed = 0;
  } else {
    DC3Speed = 0;
    DC4Speed = (int)(leftVoltage*25.5);
  }

  writePinSpeeds();

  // Prepare for next iteration
  leftDistOld = leftDist;
  rightDistOld = rightDist;
}

/*  Drives the wheels based on the provided wheel speeds, given from
 *  0.0 to 1.0 as a percentage of max PWM speed 
 */
void wheelDriveSpeed(float leftSpeed, float rightSpeed) {
  leftVoltage = leftSpeed;
  rightVoltage = rightSpeed;

  if (rightVoltage >= 0) {
    DC1Speed = (int)(abs(rightVoltage) *255);
    DC2Speed = 0;
  } else {
    DC1Speed = 0;
    DC2Speed = (int)(abs(rightVoltage)*255);
  }

  if (leftVoltage >= 0) {
    DC3Speed = (int)(abs(leftVoltage)*255);
    DC4Speed = 0;
  } else {
    DC3Speed = 0;
    DC4Speed = (int)(abs(leftVoltage)*255);
  }

  writePinSpeeds();
}

/*  Brakes the DC Motors controlling the wheels. */
void wheelBrake() {
  // Set all pins to max for magnetic braking
  DC1Speed = 255;
  DC2Speed = 255;
  DC3Speed = 255;
  DC4Speed = 255;

  writePinSpeeds();
}

/*  Drive straight for the given distance (in) at the given speed (in/s).
 *  use negative directions for reverse movement.
 */
void driveStraight(float dist, float speed) {
  int direction = (dist > 0) - (dist < 0);

  // Reset motors
  resetMotors();

  float avgSpeedDes = abs(speed);

  leftSpeedDes = direction*avgSpeedDes;
  rightSpeedDes = direction*avgSpeedDes;

  leftDistFinal = dist;
  rightDistFinal = dist;
}

/*  Rotate in place a given angular distance (deg) at a given wheel speed (in/s).
 *  Positive values are counter-clockwise and negative values are clockwise.
 */
void rotate(float dist, float speed) {
  int direction = (dist > 0) - (dist < 0);

  // Reset encoders
  resetMotors();

  float avgSpeedDes = direction*abs(speed);

  // If avgSpeed > 0, leftSpeed should be negative for CCW movement
  leftSpeedDes = -avgSpeedDes;
  rightSpeedDes = avgSpeedDes;

  leftDistFinal = -dist*PI/180.0*.5*wheelWidth;
  rightDistFinal = -leftDistFinal;
}

/*  Turn in a circular arc of the given inner-wheel radius (in) with the given average wheel speed (in/s).
 *  Positive values are counter-clockwise and negative values are clockwise.
 *  Positive speeds are for forward turns and negative speeds are for reverse.
 */
void turn(float radius, float speed) {
  float turnRadius = abs(radius);
  int direction = (speed > 0) - (speed < 0);

  float denom = 1.0/(2*turnRadius + wheelWidth);

  // Reset encoders
  resetMotors();

  float avgSpeedDes = speed;

  if (radius > 0.0) {
    leftSpeedDes = 2.0*avgSpeedDes*turnRadius*denom;
    rightSpeedDes = 2.0*avgSpeedDes*(turnRadius + wheelWidth)*denom;

    leftDistFinal = direction*PI*0.5*turnRadius;
    rightDistFinal = direction*PI*0.5*(wheelWidth + turnRadius);
  } else {
    rightSpeedDes = 2.0*avgSpeedDes*(turnRadius + wheelWidth)*denom;
    leftSpeedDes = 2.0*avgSpeedDes*turnRadius*denom;

    rightDistFinal = direction*PI*0.5*turnRadius;
    leftDistFinal = direction*PI*0.5*(wheelWidth + turnRadius);
  }
}

/*  Writes the defined analog pin speeds to the defined analog pins. */
void writePinSpeeds() {
  analogWrite(IN1, DC1Speed);
  analogWrite(IN2, DC2Speed);
  analogWrite(IN3, DC3Speed);
  analogWrite(IN4, DC4Speed);
}

/*  Checks if the wheels have reached their final calculated position. */
bool atDestination() {
  return (abs(leftDist) >= abs(leftDistFinal)) && (abs(rightDist) >= abs(rightDistFinal));
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
  leftSpeedf = 0;
  leftSpeedDes = 0;
  leftSpeedOld = 0;
  rightSpeed = 0;
  rightSpeedf = 0;
  rightSpeedDes = 0;
  rightSpeedOld = 0;

  leftDist = 0;
  leftDistOld = 0;
  leftDistDes = 0;
  leftError = 0;
  leftSpeedError = 0;
  leftIntegralError = 0;
  leftDistFinal = 0;

  rightDist = 0;
  rightDistOld = 0;
  rightDistDes = 0;
  rightError = 0;
  rightSpeedError = 0;
  rightIntegralError = 0;
  rightDistFinal = 0;
}