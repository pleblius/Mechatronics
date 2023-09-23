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

/*  Drives each wheel according to its assigned speed.
 *  Speeds should be given as a decimal fraction of max speed.
 */
void wheelDrive(float leftSpeed, float rightSpeed) {
  float leftDir = 1.;
  float rightDir = 1.;
  int leftOutput, rightOutput;

  if (leftSpeed < 0) leftDir = 0.;
  if (rightSpeed < 0) rightDir = 0.;
  
  // Get left and right tire speeds
  float leftAnalog = leftSpeed*5.0;                        
  float rightAnalog = rightSpeed*5.0;                      

  leftOutput = map(abs(leftAnalog), 0.0, 5.0, 0, 255);    
  rightOutput = map(abs(rightAnalog), 0.0, 5.0, 0, 255);  

  analogWrite(IN1, leftDir*leftOutput);
  analogWrite(IN2, (1.-leftDir)*leftOutput);
  analogWrite(IN3, rightDir*rightOutput);
  analogWrite(IN4, (1.-rightDir)*rightOutput);

  if (debugMode) {
    debugPrint("Left wheel speed: ");
    debugPrintln(leftWheelSpeed);
    debugPrint("Right wheel speed: ");
    debugPrintln(rightWheelSpeed);
  }
}

/*  Gets the speed for a wheel given the desired direction, the desired average speed, and the desired turn radius.
 *  Average speed should be given as a decimal fraction between 0 and maxMotorSpeed.
 *  Turn radius should be given as a positive floating point number.
 */
void getWheelSpeeds(Direction dir, float avgFwdSpeed, float radius, float &leftSpeed, float &rightSpeed) {

  if (debugMode) {
    debugPrintln("Getting wheel speeds for the following parameters:");
    debugPrint("Direction: "); debugPrintln(dir);
  }
  // If direction is straight, simply return desired speed
  if (dir == FORWARD || dir == REVERSE) {
    leftSpeed = dir*avgFwdSpeed;
    rightSpeed = leftSpeed;
    return;
  }
  
  // v1 = inner radius speed, v2 = outer radius speed
  float v1;
  float v2; 

  // Check divide by zero
  if (radius < .01) {
    leftSpeed = dir/2.*maxMotorSpeed;
    rightSpeed = -leftSpeed;
  }
  
  // Calculate inner and outer radius speeds, constrained to max motor speed
  v2 = (wheelBase/2. + radius)*avgFwdSpeed/radius;
  v2 = constrain(v2, 0, maxMotorSpeed);
  v1 = 2.*avgFwdSpeed - v2;

  // Assign speeds to wheels based on turn direction
  if (dir == RIGHT) {
    rightSpeed = v1;
    leftSpeed = v2;
  } else if (dir == LEFT) {
    rightSpeed = v2;
    leftSpeed = v1;
  } else {
    rightSpeed = 0.;
    leftSpeed = 0.;
  }
}

/*  Brakes the DC Motors controlling the wheels.
 */
void wheelBrake() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, HIGH);
}