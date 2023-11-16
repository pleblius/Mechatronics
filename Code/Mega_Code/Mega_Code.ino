#include <Servo.h>
#include <Encoder.h>
#include <QTRSensors.h>
#include <AccelStepper.h>
#include <MultiStepper.h>

/************************ SETUP AND GLOBALS ********************/

// Sets the program's debug mode. Robot will print to comms bus, allowing
// debugging while under automatic control.
#define OFF false
#define ON true
bool debugMode;

unsigned long timer = 0;
float dt = 0.0;
float dtTimer = 5.0;

// Holds the robot state. Use MANUAL to manually input commands for testing.
// All other states are for automatic control/competition.
enum RobotState {
  MANUAL,
  STARTUP,
  RESTARTING,
  STAGING, 
  SETUP, READY,
  PRESSING,
  READING,
  LOADING,
  ATTACHING,
  RETURNING,
  KEYBOARD
};
RobotState state;

// Struct that holds priority queues for different block types. Queue is an array that will
// pop from left to right until all values have been visited and index == size.
struct PriorityQueue {
  int index;
  int size;
  int queue[10];
};
PriorityQueue wheelQueue;
PriorityQueue batteryQueue;
PriorityQueue fanQueue;

// Block types - can be WHEEL, BATTERY, or FAN
enum Block : char {
  WHEEL = 'w',
  BATTERY = 'b',
  FAN = 'f'
};

// Global block stuff
int blockPosition;

// Represents a point in three-dimensional cartesian space.
// Has an x, y, and z coordinate.
struct Point {
  float x;
  float y;
  float z;
};

Point nextPoint;

// Traversal distance parameters
float desForwardDistance = 3.0;
float desReverseDistance = 13.0;
float actualReverseDistance = 0.0;

// Transmission variables
char rxChar;
int rxInt;

void setup() {
  wheelBrake();
  state = SETUP;
  debugMode = ON;

  // Set up Comms
  serialSetup();

  // Set up stepper motors
  stepperSetup();

  // Set up servo motors
  servoSetup();

  // Set up DC motors
  motorSetup();

  // Set up line sensor array
  arraySetup();
  
  // Set up distance sensors
  distanceSetup();

  // Set up electromagnet
  magnetSetup();

  // Set up color detector
  colorSetup();
}

void loop() {
  // Loop timing
  unsigned static long lastUpdate;
  unsigned long curTime = millis();
  dt = .001*(curTime - lastUpdate);
  

  switch (state) {
    case RESTARTING: {
      runSteppers();

      if (finishedStepping()) {
        // Restart code 1 - finished restarting
        sendTransmission('R', 1);

        state = STARTUP;
      }
    }

    case STARTUP: {
      // Start code 0 - begin startup
      sendTransmission('S', 0);
      // Block code 0 - prompt user
      sendTransmission('B', 0);

      state = SETUP;
    } break;

    case SETUP: {
      if (receiveTransmission()) {
        if (restartTriggered()) {
          restart();
        }
        // If invalid block location, exit block early
        else if (rxInt > 20) {
          // Start code 19 - invalid block number
          sendTransmission('B', 19);
        }
        // If queue hasn't been built yet, check for valid-queue build command and initiate build
        else if (!isQueueBuilt()) {
          switch (rxChar) {
            case 'W':
            case 'w': {
              buildQueue(WHEEL, rxInt);
            } break;

            case 'F':
            case 'f': {
              buildQueue(FAN, rxInt);
            } break;

            case 'B':
            case 'b': {
              buildQueue(BATTERY, rxInt);
            } break;

            default: {
              // Error code 9 - invalid block type
              sendTransmission('B', 9);
            }
          }
        }
        // If queue is already built, update queue with new attached block
        else if (isQueueBuilt()) {
          updateQueue(rxInt);
        }
        else {
          // Error code 99 - unknown state
          sendTransmission('X', 99);
        }
      }
    } break;

    case STAGING: {
      runSteppers();

      if (finishedStepping()) {
        state = READY;
        Serial.println("\nARM STAGED\n");
      }
    }
    
    case READY: {
      timer = 0;
      state = PRESSING;
      Serial.println("\nPRESSING\n");
    } break;

    case PRESSING: {
      rotatePresser(30);

      // Wait 2 seconds, swap to reading - skip for PM
      if (timer > 2000) {
        timer = 0;
        state = LOADING;
        rotatePresser(0);

        Serial.println("\nLOADING\n");

        nextPoint = getBlockCatchUp();
        Serial.println("\nNext Point:");
        Serial.print(nextPoint.x); Serial.print(" "); Serial.print(nextPoint.y); Serial.print(" "); Serial.println(nextPoint.z);
        moveToNextPoint();
      }

    } break;

    case READING: {
      // Read block color
      // Get block position
    } break;

    case LOADING: {
      runSteppers();
      static bool up = true;

      if (finishedStepping() && up) {
        nextPoint = getBlockCatchDown();
        Serial.println("\nNext Point:");
        Serial.print(nextPoint.x); Serial.print(" "); Serial.print(nextPoint.y); Serial.print(" "); Serial.println(nextPoint.z);

        moveToNextPoint();
        up = false;

        activateMagnet();
      } else if (finishedStepping() && !up) {
        
        up = true;
        nextPoint = getBlockCatchUp();

        Serial.println("\nNext Point:");
        Serial.print(nextPoint.x); Serial.print(" "); Serial.print(nextPoint.y); Serial.print(" "); Serial.println(nextPoint.z);

        moveToNextPoint();

        approachPosition(blockPosition);
        Serial.println("\nAWAITING COMMAND TO PLACE BLOCK\n");

        state = KEYBOARD;
      }
    } break;

    case ATTACHING: {
      runSteppers();

      if (finishedStepping()) {

        if (finishedApproaching()) {

          deactivateMagnet();
          approachReturn();
          state = RETURNING;

        } else {
          nextPoint = getNextPoint();
          Serial.println("\nNext Point:");
          Serial.print(nextPoint.x); Serial.print(" "); Serial.print(nextPoint.y); Serial.print(" "); Serial.println(nextPoint.z);

          moveToNextPoint();
        }
      }

    } break;

    case RETURNING: {
      if (finishedStepping()) {
        if (finishedReturning()) {

          // sendTransmission('M', 1);
          state = KEYBOARD;

          Serial.println("\nDone.\n");
          Serial.println("\nAwaiting command for autonomous operations.\n");

        } else {
          nextPoint = getNextPoint();

          Serial.println("\nNext Point:");
          Serial.print(nextPoint.x); Serial.print(" "); Serial.print(nextPoint.y); Serial.print(" "); Serial.println(nextPoint.z);

          moveToPos(nextPoint.x, nextPoint.y, nextPoint.z);
        }
      }

      runSteppers();
    } break;

    case KEYBOARD: {
      runSteppers();
      char c;

      if (Serial.available()) {
        c = Serial.read();

        switch (c) {
          case 'G': {}
          case 'g': {
            state = STAGING;
            nextPoint = getStaging();
            moveToNextPoint();

            Serial.println("\nGRABBING\n");

          } break;

          case 'A': {}
          case 'a': {
            state = ATTACHING;

            Serial.println("\nATTACHING\n");
          } break;

          default: {}
        }
      }
    }
    
    default: {

    }
  }

  if (timer > dtTimer) {
    timer = 0;
  }
}

void moveToNextPoint() {
  moveToPos(nextPoint.x, nextPoint.y, nextPoint.z);
}

/*  Restarts all robot operations. */
void restart() {
  // Restart code 0 - initiating restart
  sendTransmission('R', 0);

  deactivateMagnet();

  // Reset arm position
  moveToHome();

  // Reset wheel motors
  resetMotors();

  // Reset block queue
  resetQueues();

  state = RESTARTING;
}

/*  Checks if a restart command has been received. */
bool restartTriggered() {
  return (rxChar == 'R' || rxChar == 'r');
}