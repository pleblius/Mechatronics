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
  STARTUP,
  RESTARTING,
  SETUP,
  EXITING,
  ACQUIRING,
  READING,
  BACKING,
  FRONTING,
  DISCARDING,
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

  // Get IR sensor voltage every loop for filtering.
  getFrontVoltage();

  switch (state) {
    // Resets the robot to preprare for competition restart.
    case RESTARTING: {
      restartOperations();
    } break;

    // Basic startup commands for when robot is turned on/reset
    case STARTUP: {
      // Block code 0 - prompt user for block input
      sendTransmission('B', 0);

      setupService();
    } break;

    // Block queue setup/update commands. Starts competition when proper command is received.
    case SETUP: {
      setupOperations();
    } break;

    // Exits the starting stall by following the black line. Checks for restart commands.
    case EXITING: {
      // Fraction of max speed
      float speed;
      float distance = getFrontDistance();

      // Ignore intersections
      if (atIntersectionFront()) {
        return;
      }

      // Slowly approach the button
      if (distance < desForwardDistance) {
        wheelBrake();

        driveStraight(1, 1);
        state = ACQUIRING;
        return;
      } else if (distance < 5.0) {
        speed = (distance - desForwardDistance)*0.2;
      } else {
        speed = 0.4;
      }

      forwardFollow(speed);

      restartCheck();

    } break;

    // Acquires a block by nudging the button and backing up slightly. Checks for restarts.
    case ACQUIRING: {
      static bool forwardFlag = true;

      if (atDestination()) {
        if (forwardFlag) {
          forwardFlag = false;
          driveStraight(-1, 1);
        } else {
          forwardFlag = true;

          wheelBrake();
          state = READING;
        }
      }

      restartCheck();
    } break;

    // Checks the block's color and gets the next placement location. Checks for restarts.
    case READING: {
      Block block;

      if (isRed()) {
        block = WHEEL;
      } else if (isBlue()) {
        block = FAN;
      } else if (isYellow()) {
        block = BATTERY;
      }
      // If reading fails reset to acquiring
      else {
        driveStraight(1, 1);
        state = ACQUIRING;
        return;
      }

      blockPosition = getNextPosition(block);

      // If block is a throwaway, discard it
      if (blockPosition == -1) {
        discardApproach();
        nextPoint = getNextPoint();
        moveToNextPoint();
        state = DISCARDING;
      }
      restartCheck();
    } break;

    // Reverse away from the block dispenser towards the vehicle chassis.
    case BACKING: {
      static bool firstLeg = true;


    } break;

    case DISCARDING: {

    } break;

    case FRONTING: {

    } break;

    // case LOADING: {
    //   runSteppers();
    //   static bool up = true;

    //   if (finishedStepping() && up) {
    //     nextPoint = getBlockCatchDown();
    //     Serial.println("\nNext Point:");
    //     Serial.print(nextPoint.x); Serial.print(" "); Serial.print(nextPoint.y); Serial.print(" "); Serial.println(nextPoint.z);

    //     moveToNextPoint();
    //     up = false;

    //     activateMagnet();
    //   } else if (finishedStepping() && !up) {
        
    //     up = true;
    //     nextPoint = getBlockCatchUp();

    //     Serial.println("\nNext Point:");
    //     Serial.print(nextPoint.x); Serial.print(" "); Serial.print(nextPoint.y); Serial.print(" "); Serial.println(nextPoint.z);

    //     moveToNextPoint();

    //     approachPosition(blockPosition);
    //     Serial.println("\nAWAITING COMMAND TO PLACE BLOCK\n");

    //     state = KEYBOARD;
    //   }
    // } break;

    // case ATTACHING: {
    //   runSteppers();

    //   if (finishedStepping()) {

    //     if (finishedApproaching()) {

    //       deactivateMagnet();
    //       approachReturn();
    //       state = RETURNING;

    //     } else {
    //       nextPoint = getNextPoint();
    //       Serial.println("\nNext Point:");
    //       Serial.print(nextPoint.x); Serial.print(" "); Serial.print(nextPoint.y); Serial.print(" "); Serial.println(nextPoint.z);

    //       moveToNextPoint();
    //     }
    //   }

    // } break;

    // case RETURNING: {
    //   if (finishedStepping()) {
    //     if (finishedReturning()) {

    //       // sendTransmission('M', 1);
    //       state = KEYBOARD;

    //       Serial.println("\nDone.\n");
    //       Serial.println("\nAwaiting command for autonomous operations.\n");

    //     } else {
    //       nextPoint = getNextPoint();

    //       Serial.println("\nNext Point:");
    //       Serial.print(nextPoint.x); Serial.print(" "); Serial.print(nextPoint.y); Serial.print(" "); Serial.println(nextPoint.z);

    //       moveToPos(nextPoint.x, nextPoint.y, nextPoint.z);
    //     }
    //   }

    //   runSteppers();
    // } break;

    // case KEYBOARD: {
    //   runSteppers();
    //   char c;

    //   if (Serial.available()) {
    //     c = Serial.read();

    //     switch (c) {
    //       case 'G': {}
    //       case 'g': {
    //         state = STAGING;
    //         nextPoint = getStaging();
    //         moveToNextPoint();

    //         Serial.println("\nGRABBING\n");

    //       } break;

    //       case 'A': {}
    //       case 'a': {
    //         state = ATTACHING;

    //         Serial.println("\nATTACHING\n");
    //       } break;

    //       default: {}
    //     }
    //   }
    // }
    
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

/*  Simple restart command checker. */
void restartCheck() {
  if (receiveTransmission()) {
    if (restartTriggered()) {
      restartService();
    }
  }
}