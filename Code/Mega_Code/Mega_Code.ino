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
  FORWARD,
  LOADING,
  PLACING
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
bool discard;

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

      // Robot has closed the distance - set state to acquiring
      if (distance < desForwardDistance) {
        acquiringService();
        return;
      }
      // Robot is closing on button - slow approach
      else if (distance < 5.0) {
        speed = (distance - desForwardDistance)*0.2;
      }
      // Normal line-follow
      else {
        speed = 0.4;
      }

      forwardFollow(speed);

      restartCheck();
    } break;

    // Acquires a block by nudging the button and backing up slightly. Checks for restarts.
    case ACQUIRING: {
      static bool forwardFlag = true;

      wheelDrive();

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
        acquiringService();
        return;
      }

      blockPosition = getNextPosition(block);

      // If block is a throwaway, discard it
      if (blockPosition == -1) {
        loadingService();
        discard = true;
      } else {
        discard = false;
        state = BACKING;
      }
      restartCheck();
    } break;

    // Reverse away from the block dispenser towards the vehicle chassis.
    case BACKING: {
      backingOperations();
    } break;

    case FORWARD: {
      forwardOperations();
    } break;

    case PLACING: {
      placingOperations();
    } break;

    case LOADING: {
      runSteppers();
      static bool up = true;

      // If arm is above block-catch point, begin dropping arm
      if (finishedStepping() && up) {
        nextPoint = getBlockCatchDown();

        moveToNextPoint();
        up = false;

        activateMagnet();
      }
      // If arm has collected block
      else if (finishedStepping() && !up) {
        
        up = true;
        nextPoint = getBlockCatchUp();

        moveToNextPoint();

        placingService();
      }

      restartCheck();
    } break;

    default: {
      restartCheck();
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

/*  Service to initiate block procurement. */
void acquiringService() {
  driveStraight(1, 1);
  state = ACQUIRING;
}