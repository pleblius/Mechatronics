/*  Controls robot behavior while in the manual control operational state.
 *  Can only perform one operation at a time.
 */
void manualOperations(float dt) {
  static float timer;
  static int duration;
  static int desiredDistance;

  switch (manualState) {
    case WAITING: {
      
      if (receiveTransmission()) {
        switch (rxChar) {
          case 'D':
          case 'd': {
            getWheelSpeeds(FORWARD, 0.1, 0.0);

            manualState = FOLLOWING;
            desiredDistance = rxInt;

            wheelDrive();

            sendTransmission('M', 10);

          } break;

          case 'F':
          case 'f': {
            // Drive forward for int seconds
            getWheelSpeeds(FORWARD, 0.25, 0.0);
            duration = rxInt;
            
            manualState = MOVING;
            wheelDrive();
            
            sendTransmission('M', 2);
          } break;

          case 'B':
          case 'b': {
            // Drive in reverse - int seconds
            getWheelSpeeds(REVERSE, 0.25, 0.0);
            duration = rxInt;

            manualState = MOVING;
            wheelDrive();

            sendTransmission('M', 3);
          } break;

          case 'L':
          case 'l': {
            // Turn left - int seconds
            getWheelSpeeds(LEFT, 0, 0);
            duration = rxInt;

            manualState = ROTATING;
            wheelDrive();

            sendTransmission('M', 4);
          } break;

          case 'R':
          case 'r': {
            // Turn right - int seconds
            getWheelSpeeds(RIGHT, 0, 0);
            duration = rxInt;

            manualState = ROTATING;
            wheelDrive();

            sendTransmission('M', 5);
          } break;

          case 'T':
          case 't': {
            // moveTo turret int degrees; <180 for left, >180 for right
            manualState = TURRET;

            int moveDist;
            
            if (rxInt <= 90) {
              moveDist = 95*rxInt/stepSize;
            }
            else if (rxInt <= 180) {
              moveDist = -95*(rxInt - 90)/stepSize;
            }

            turretMotor.move(moveDist);

            sendTransmission('M', 6);
          } break;

          case 'A':
          case 'a': {
            // moveTo second link int degrees; <180 for left, >180 for right
            manualState = ARM;

            int moveDist;
            
            if (rxInt <= 90) {
              moveDist = 50*rxInt/stepSize;
            }
            else if (rxInt <= 180) {
              moveDist = -50*(rxInt - 90)/stepSize;
            }

            armMotor.move(moveDist);

            sendTransmission('M', 7);
          } break;

          case 'Z':
          case 'z': {
            // moveTo arm to int position; will do nothing if already in position
            manualState = TOWER;
            
            int moveDist;
            
            if (rxInt <= 90) {
              moveDist = 80*rxInt/stepSize;
            }
            else if (rxInt <= 180) {
              moveDist = -80*(rxInt - 90)/stepSize;
            }

            zMotor.move(moveDist);

            sendTransmission('M', 9);

          } break;

          default: {
            sendTransmission('M', 0);
          } break;
        }

        timer = 0.0;
      }
    } break;

    case FOLLOWING: {
      float distance = getFrontDistance();

      static long debugTimer = millis() - 1001;
      if (debugMode && millis() - debugTimer > 1000) {
        debugPrintln("Speeds:");
        debugPrint(leftWheelSpeed);
        debugPrint(" ");
        debugPrint(rightWheelSpeed);
        debugPrintln("Distance:");
        debugPrintln(distance);
        debugPrintln("");

        debugTimer = millis();
      }

      if (distance < desiredDistance) {
        wheelBrake();

        manualState = WAITING;

        sendTransmission('M', 1);
      }

      lineFollow(0.1);

    } break;

    case MOVING: {
      if (debugMode) {
        debugPrint(timer);
        debugPrint(" ");
        debugPrintln(dt);
      }

      timer += dt;

      if (timer > duration) {
        wheelBrake();

        manualState = WAITING;
        
        sendTransmission('M', 1);
      }

    } break;

    case ROTATING: {
      wheelDrive();

      timer += dt;

      if (timer > duration) {
        wheelBrake();

        manualState = WAITING;

        sendTransmission('M', 1);
      }

    } break;

    case ARM: {
      if (armMotor.distanceToGo() == 0) {
        manualState = WAITING;

        sendTransmission('M', 1);
      } 
    } break;

    case TURRET: {
      if (turretMotor.distanceToGo() == 0) {
        manualState = WAITING;

        sendTransmission('M', 1);
      } 
    } break;

    case TOWER: {
      if (zMotor.distanceToGo() == 0) {
        manualState = WAITING;

        sendTransmission('M', 1);
      } 
    } break;
  }
}