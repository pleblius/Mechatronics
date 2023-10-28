enum ManualState {
  WAITING,
  MOVING,
  ROTATING,
  ARCING,
  FOLLOWING,
  ARM,
  TURRET,
  TOWER
};
enum ManualState manualState = WAITING;

float desiredDistance;

/*  Controls robot behavior while in the manual control operational state.
 *  Can only perform one operation at a time.
 */
void manualOperations() {
  static unsigned long lastUpdate = 0;
  float dt = float(millis() - lastUpdate)/1000.0;
  lastUpdate = millis();

  switch (manualState) {
    case WAITING: {
      
      if (receiveTransmission()) {
        switch (rxChar) {
          // Drive straight
          case 'D':
          case 'd': {
            // Store distance to drive
            desiredDistance = rxInt;

            while (!receiveTransmission()) {
              delay(1000);
            }

            switch (rxChar) {
              case 'F':
              case 'f': {
                driveStraight(desiredDistance, rxInt);
              } break;
              case 'R':
              case 'r': {
                driveStraight(-desiredDistance, rxInt);
              } break;
            }

            manualState = MOVING;
            sendTransmission('M', 2);
          } break;
          
          // Rotate in place
          case 'R':
          case 'r': {
            // Store turn angle
            desiredDistance = rxInt;

            while (!receiveTransmission()) {
              delay(1000);
            }

            switch (rxChar) {
              case 'R':
              case 'r': {
                rotate(-desiredDistance, rxInt);
              } break;
              case 'L':
              case 'l': {
                rotate(desiredDistance, rxInt);
              } break;
            }

            manualState = ROTATING;
            sendTransmission('M', 3);
          } break;

          // Turn in a circular arc
          case 'C':
          case 'c': {
            // Store turn radius (cm)
            desiredDistance = rxInt;

            while (!receiveTransmission()) {
              delay(1000);
            }

            switch (rxChar) {
              case 'R':
              case 'r': {
                turn(-desiredDistance, rxInt);
              } break;
              case 'L':
              case 'l': {
                turn(desiredDistance, rxInt);
              } break;
            }

            manualState = ARCING;
            sendTransmission('M', 4);
          } break;

          case 'L':
          case 'l': {
            // Store distance sensor min distance
            desiredDistance = rxInt;

            manualState = FOLLOWING;
          } break;

          case 'T':
          case 't': {
            // moveTo turret int degrees; <180 for left, >180 for right
            manualState = TURRET;

            if (rxInt <= 90) {
              moveTurretToAngle(rxInt);
            }
            else if (rxInt <= 180) {
              moveTurretToAngle(-(rxInt - 90));
            }

            sendTransmission('M', 6);
          } break;

          case 'A':
          case 'a': {
            // moveTo second link int degrees; <180 for left, >180 for right
            manualState = ARM;

            if (rxInt <= 90) {
              moveArmToAngle(rxInt);
            }
            else if (rxInt <= 180) {
              moveArmToAngle(-(rxInt - 90));
            }

            sendTransmission('M', 7);
          } break;

          case 'Z':
          case 'z': {
            // moveTo arm to int position; will do nothing if already in position
            manualState = TOWER;
            
            if (rxInt <= 90) {
              moveZToPos(rxInt/90.0);
            }
            else if (rxInt <= 180) {
              moveZToPos(-(rxInt - 90.0)/90.0);
            }

            sendTransmission('M', 9);

          } break;

          default: {
            sendTransmission('M', 0);
          } break;
        }
      }
    } break;

    case FOLLOWING: {
      float distance = getFrontDistance();

      if (distance < desiredDistance) {
        wheelBrake();

        manualState = WAITING;

        sendTransmission('M', 2);

        return;
      }

      // Line follow at 25% speed.
      lineFollow(0.25);

    } break;

    case ROTATING: {}
    case ARCING: {}
    case MOVING: {
      if (!wheelDrive()) {
        manualState = WAITING;

        sendTransmission('M', 1);
      }
    } break;

    case ARM: {}
    case TURRET: {}
    case TOWER: {
      if (finishedStepping()) {
        manualState = WAITING;

        sendTransmission('M', 1);
      }

      runSteppers();
    } break;
  }
}