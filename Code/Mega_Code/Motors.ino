// PINS
#define IN1 9
#define IN2 10
#define IN3 11
#define IN4 12

#define CENA 45
#define CIN1 47
#define CIN2 49

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
void wheelDrive() {
  getPinSpeeds();

  analogWrite(IN1, DC1Speed);
  analogWrite(IN2, DC2Speed);
  analogWrite(IN3, DC3Speed);
  analogWrite(IN4, DC4Speed);
}

/*  Gets the speed for a wheel given the desired direction, the desired average speed, and the desired turn radius.
 *  Average speed should be given as a decimal fraction between 0 and maxMotorSpeed.
 *  Turn radius should be given as a positive floating point number.
 */
void getWheelSpeeds(Direction dir, float avgFwdSpeed, float radius) {

  if (debugMode) {
    debugPrintln("Getting wheel speeds for the following parameters:");
    debugPrint("Direction: "); debugPrintln(dir);
  }
  
  // If direction is straight, simply return desired speed
  if (dir == FORWARD || dir == REVERSE) {
    leftWheelSpeed = dir*avgFwdSpeed;
    rightWheelSpeed = leftWheelSpeed;

    if (debugMode) {
      debugPrint("Left wheel speed: ");
      debugPrintln(leftWheelSpeed);
      debugPrint("Right wheel speed: ");
      debugPrintln(rightWheelSpeed);
    }

    return;
  }
  
  // v1 = inner radius speed, v2 = outer radius speed
  float v1;
  float v2; 

  // Check divide by zero
  if (radius < .01) {
    leftWheelSpeed = dir/2.*maxMotorSpeed;
    rightWheelSpeed = -leftWheelSpeed;

    if (debugMode) {
      debugPrint("Left wheel speed: ");
      debugPrintln(leftWheelSpeed);
      debugPrint("Right wheel speed: ");
      debugPrintln(rightWheelSpeed);
    }

    return;
  }
  
  // Calculate inner and outer radius speeds, constrained to max motor speed
  v2 = (wheelBase/2. + radius)*avgFwdSpeed/radius;
  v2 = constrain(v2, 0, maxMotorSpeed);
  v1 = 2.*avgFwdSpeed - v2;

  // Assign speeds to wheels based on turn direction
  if (dir == RIGHT) {
    rightWheelSpeed = v1;
    leftWheelSpeed = v2;
  } else if (dir == LEFT) {
    rightWheelSpeed = v2;
    leftWheelSpeed = v1;
  } else {
    rightWheelSpeed = 0.;
    leftWheelSpeed = 0.;
  }

  if (debugMode) {
    debugPrint("Left wheel speed: ");
    debugPrintln(leftWheelSpeed);
    debugPrint("Right wheel speed: ");
    debugPrintln(rightWheelSpeed);
  }
}

/*  Converts the desired wheel speeds into analog pin speeds.
 *  Caches values in global variables for faster computation and smaller loop deltas.
 */
void getPinSpeeds() {
  float leftDir = 1.;
  float rightDir = 1.;

  if (leftWheelSpeed < 0) leftDir = 0.;
  if (rightWheelSpeed < 0) rightDir = 0.;
  
  // Get left and right tire speeds
  float leftAnalog = leftWheelSpeed*5.0;                        
  float rightAnalog = rightWheelSpeed*5.0;                      

  int leftOutput = (int)(leftAnalog*255.0/5.0);
  int rightOutput = (int)(rightAnalog*255.0/5.0);

  DC2Speed = leftDir*leftOutput;
  DC1Speed = (1.-leftDir)*leftOutput;
  DC4Speed = (1.-rightDir)*rightOutput;
  DC3Speed = rightDir*rightOutput;

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