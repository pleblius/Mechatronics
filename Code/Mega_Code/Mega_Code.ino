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
unsigned long dt = 0;
unsigned long dtTimer = 5000;

// Holds the robot state. Use MANUAL to manually input commands for testing.
// All other states are for automatic control/competition.
enum RobotState {
  MANUAL,
  STARTUP,
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

  // Set up arm movement locations
  loadPoint1();
}

void loop() {
  // Loop timing
  unsigned static long lastUpdate;
  dt = millis() - lastUpdate;
  lastUpdate = millis();
  timer += dt;

  switch (state) {
    case STARTUP: {
      // Ping uno every five seconds until reponse is received.
      if (timer > 5000) {
        checkManualControl();
        timer = 0;
      }

    } break;

    case MANUAL: {
      manualOperations();
    } break;

    case SETUP: {
      runSteppers();
      
      if (finishedStepping()) {
        state = KEYBOARD;
        Serial.println("\nAwaiting command for autonomous operations.\n");
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
          
          loadPoint2();

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