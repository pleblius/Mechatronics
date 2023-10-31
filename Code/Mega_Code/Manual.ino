enum ManualState {
  WAITING,
  MOVING,
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

  switch (manualState) {
    case WAITING: {
      
      if (receiveTransmission()) {
        switch (rxChar) {
          // Drive straight
          case 'D':
          case 'd': {
            // Store distance to drive
            desiredDistance = rxInt;

            sendTransmission('M', 2);

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
              default: {
                sendTransmission('X', 100);
              } break;
            }

            manualState = MOVING;
            sendTransmission('M', 12);

          } break;
          
          // Rotate in place
          case 'R':
          case 'r': {
            // Store turn angle
            desiredDistance = rxInt;

            sendTransmission('M', 3);

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
              default: {
                sendTransmission('X', 100);
              } break;
            }

            manualState = MOVING;
            sendTransmission('M', 13);
          } break;

          // Turn in a circular arc
          case 'C':
          case 'c': {
            // Store turn radius (cm)
            desiredDistance = rxInt;

            sendTransmission('M', 4);

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
              default: {
                sendTransmission('X', 100);
              } break;
            }

            manualState = MOVING;
            sendTransmission('M', 14);
          } break;

          case 'L':
          case 'l': {
            // Store distance sensor min distance
            desiredDistance = rxInt;

            manualState = FOLLOWING;

            sendTransmission('M', 5);
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

            sendTransmission('M', 8);

          } break;

          default: {
            sendTransmission('X', 100);
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

    case MOVING: {
      if (atDestination()) {
        resetMotors();

        wheelBrake();

        sendTransmission('M',1);
        manualState = WAITING;
      }

      wheelDrive();
    } break;

    case ARM: {} break;
    case TURRET: {} break;
    case TOWER: {
      if (finishedStepping()) {
        manualState = WAITING;

        sendTransmission('M', 1);
      }

      runSteppers();
    } break;
  }
}