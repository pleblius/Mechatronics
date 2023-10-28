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

float leftAngle = 0.0;
float leftAngleOld = 0.0;
float leftAngleDes;
float leftAngleFinal;

float rightAngle = 0.0;
float rightAngleOld = 0.0;
float rightAngleDes;
float rightAngleFinal;

// Encoders
Encoder leftEncoder(ENCL1, ENCL2);
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

float wheelRadius = 4.2;
float degPerCm = 180.0/wheelRadius/pi;
float wheelDistance = 26.0;
float bodyWheelRatio = wheelDistance*0.5/wheelRadius;

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
}

/*  Wheel driving loop function. Updates position and velocity via encoder, then calculates new velocity
 *  based on closed-loop control for the desired drive trajectory.
 *  Call every loop to re-calculate drive characteristics for closed-loop control while dead-reckoning.
 *  Returns true while the wheels have distance to cover, and false when they have reached their desired angle.
 */
bool wheelDrive() {
  static unsigned long lastUpdate = 0;
  float dt = float(millis() - lastUpdate)/1000.0;
  lastUpdate = millis();

  // Check if destination reached
  if (leftAngleDes == leftAngleFinal && rightAngleDes == rightAngleFinal) {
    return true;
  }

  // Update wheel position and velocity
  leftCount = leftEncoder.read();
  rightCount = rightEncoder.read();

  leftAngle = leftCount*degPerCount;
  rightAngle = rightCount*degPerCount;

  leftSpeed = (leftAngle - leftAngleOld)/dt;
  rightSpeed = (rightAngle - rightAngleOld)/dt;

  // Get new desired position
  leftAngleDes += leftSpeedDes*dt;
  rightAngleDes += rightSpeedDes*dt;

  // Get desired wheel speeds from closed-loop equation
  leftOutput = Kp*(leftAngleDes - leftAngle);
  rightOutput = Kp*(rightAngleDes - rightAngle);

  // Constrain to max speed (will constrain both motors and keep the speed ratios the same)
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

  // Get and output necessary drive voltage
  getPinSpeeds();

  analogWrite(IN1, DC1Speed);
  analogWrite(IN2, DC2Speed);
  analogWrite(IN3, DC3Speed);
  analogWrite(IN4, DC4Speed);

  leftAngleOld = leftAngle;
  rightAngleOld = rightAngle;

  return false;
}

/*  Drives the wheels based on the provided wheel speeds, given from
 *  0.0 to 1.0 as a percentage of max PWM speed 
 */
void wheelDriveSpeed(float leftSpeed, float rightSpeed) {
  leftOutput = leftSpeed;
  rightOutput = rightSpeed;

  getPinSpeeds();

  analogWrite(IN1, DC1Speed);
  analogWrite(IN2, DC2Speed);
  analogWrite(IN3, DC3Speed);
  analogWrite(IN4, DC4Speed);
}

/*  Converts the desired wheel speeds into analog pin speeds.
 */
void getPinSpeeds() {
  // Checks wheel direction and adjusts which pins are high and low accordingly
  int leftDir = 1;
  int rightDir = 1;

  if (leftOutput < 0) leftDir = 0;
  if (rightOutput < 0) rightDir = 0;
  
  // Get PWM output ratio from output speed
  float leftAnalog = leftOutput*5.0;                        
  float rightAnalog = rightOutput*5.0;                      

  // Convert PWM to analogWrite
  int leftWrite = (int)leftAnalog*51;
  int rightWrite = (int)rightAnalog*51;

  // Set pin speeds to analogWrite values
  DC2Speed = leftDir*leftWrite;
  DC1Speed = (1-leftDir)*leftWrite;
  DC4Speed = (1-rightDir)*rightWrite;
  DC3Speed = rightDir*rightWrite;

  if (debugMode) {
    debugPrintln("Pin speeds:");
    debugPrintln(DC1Speed);
    debugPrintln(DC2Speed);
    debugPrintln(DC3Speed);
    debugPrintln(DC4Speed);
  }
}

/*  Brakes the DC Motors controlling the wheels.
 */
void wheelBrake() {
  DC1Speed = 255;
  DC2Speed = 255;
  DC3Speed = 255;
  DC4Speed = 255;

  analogWrite(IN1, DC1Speed);
  analogWrite(IN2, DC2Speed);
  analogWrite(IN3, DC3Speed);
  analogWrite(IN4, DC4Speed);

  if (debugMode) {
    Serial.println("Braking.");
  }
}

/*  Drive straight for the given distance (cm) at the given speed (cm/s).
 *  use negative directions for reverse movement.
 */
void driveStraight(float dist, float speed) {
  int direction = (dist > 0) - (dist < 0);
  // Convert speed from cm/s to deg/s
  avgSpeedDes = direction*abs(speed)*degPerCm;

  leftSpeedDes = avgSpeedDes;
  rightSpeedDes = avgSpeedDes;

  // Convert distances from cm to deg
  leftAngleFinal = dist*degPerCm;
  rightAngleFinal = leftAngleFinal;
}

/*  Rotate in place a given angular distance (deg) at a given speed (deg/s).
 *  Positive values are counter-clockwise and negative values are clockwise.
 */
void rotate(float dist, float speed) {
  avgSpeedDes = speed*bodyWheelRatio;

  // If avgSpeed > 0, leftSpeed should be negative for CCW movement
  leftSpeedDes = -avgSpeedDes;
  rightSpeedDes = avgSpeedDes;

  leftAngleFinal = -dist*bodyWheelRatio;
  rightAngleFinal = -leftAngleFinal;
}

/*  Turn in a circular arc of the given inner-wheel radius (cm) with the given average wheel speed (cm/s).
 *  Positive values are counter-clockwise and negative values are clockwise.
 */
void turn(float radius, float speed) {
  float turnRadius = abs(radius);

  float bodyTurnRatio = (wheelDistance + turnRadius)/turnRadius;
  float wheelTurnRatio = turnRadius/wheelRadius;

  // Convert speed from cm/s to deg/s
  avgSpeedDes = speed*degPerCm;

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