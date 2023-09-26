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

      if (wirelessRead()) {
        switch (rxChar) {
          case 'F':
          case 'f': {
            // Drive forward for int seconds
            getWheelSpeeds(FORWARD, 0.25, 0.0);
            duration = rxInt;
            
            manualState = DRIVING;
            
            sendTransmission('M', 2);
          } break;

          case 'B':
          case 'b': {
            // Drive in reverse - int seconds
            getWheelSpeeds(REVERSE, 0.25, 0.0);
            duration = rxInt;

            manualState = DRIVING;

            sendTransmission('M', 3);
          } break;

          case 'L':
          case 'l': {
            // Turn left - int seconds
            getWheelSpeeds(LEFT, 0, 0.1);
            duration = rxInt;

            manualState = TURNING;

            sendTransmission('M', 4);
          } break;

          case 'R':
          case 'r': {
            // Turn right - int seconds
            getWheelSpeeds(RIGHT, 0, 0.1);
            duration = rxInt;

            manualState = TURNING;

            sendTransmission('M', 5);
          } break;

          case 'T':
          case 't': {
            // Move turret int degrees; <180 for left, >180 for right
            manualState = PLACING;

            sendTransmission('M', 6);
          } break;

          case 'A':
          case 'a': {
            // Move second link int degrees; <180 for left, >180 for right
            manualState = PLACING;

            sendTransmission('M', 7);
          } break;

          case 'W':
          case 'w': {
            // Move wrist link int degrees; <180 for left, >180 for right
            manualState = PLACING;

            sendTransmission('M', 8);
          } break;

          case 'Z':
          case 'z': {
            // Move arm to int position; will do nothing if already in position
            manualState = PLACING;

            sendTransmission('M', 9);
          } break;

          default: break;
        }

        timer = 0L;
      }
    } break;

    case DRIVING: {
      wheelDrive();

      timer += dt;
      
      if (timer > duration) {
        wheelBrake();

        manualState = READY;
        
        sendTransmission('M', 1);
      }

    } break;

    case TURNING: {
      wheelDrive();

      timer += dt;

      if (timer > duration) {
        wheelBrake();

        manualState = READY;

        sendTransmission('M', 1);
      }

    } break;

    case PLACING: {

    } break;
  }
}