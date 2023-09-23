/*  Controls robot behavior while in the manual control operational state.
 *  Can only perform one operation at a time.
 */
void manualOperations(unsigned int dt) {
  static unsigned long timer;
  static unsigned int duration;

  switch (manualState) {
    case READY: {
      // User input variables
      char chInput;
      int intInput;

      if (wirelessRead(chInput, intInput)) {
        switch (chInput) {
          case 'f': {
            // Drive forward for int seconds
            getWheelSpeeds(FORWARD, 0, 0.5, leftWheelSpeed, rightWheelSpeed);
            duration = intInput;
            
            manualState = DRIVING;
          } break;

          case 'b': {
            // Drive in reverse - int seconds
            getWheelSpeeds(REVERSE, 0, 0.5, leftWheelSpeed, rightWheelSpeed);
            duration = intInput;

            manualState = DRIVING;
          } break;

          case 'l': {
            // Turn left - int seconds
            getWheelSpeeds(LEFT, 0, 0.1, leftWheelSpeed, rightWheelSpeed);
            duration = intInput;

            manualState = TURNING;
          } break;

          case 'r': {
            // Turn right - int seconds
            getWheelSpeeds(RIGHT, 0, 0.1, leftWheelSpeed, rightWheelSpeed);
            duration = intInput;

            manualState = TURNING;
          } break;

          case 'z': {
            // Move arm to int position; will do nothing if already in position
            manualState = PLACING;
          } break;

          case 't': {
            // Move turret int degrees; <180 for left, >180 for right
            manualState = PLACING;
          } break;

          case 'a': {
            // Move second link int degrees; <180 for left, >180 for right
            manualState = PLACING;
          } break;

          case 'w': {
            // Move wrist link int degrees; <180 for left, >180 for right
            manualState = PLACING;
          } break;

          default: break;
        }

        timer = 0L;
      }
    } break;

    case DRIVING: {
      wheelDrive(leftWheelSpeed, rightWheelSpeed);

      timer += dt;
      if (timer > duration) {
        wheelBrake();

        manualState = READY;
        debugPrintln("Please enter input: [char] [int]");
      }

    } break;

    case TURNING: {
      wheelDrive(leftWheelSpeed, rightWheelSpeed);

      timer += dt;
      if (timer > duration) {
        wheelBrake();

        manualState = READY;
        debugPrintln("Please enter input: [char] [int]");
      }

    } break;

    case PLACING: {

    } break;
  }
}