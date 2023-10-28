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

float pi = 3.1416;

// Holds the robot state. Use MANUAL to manually input commands for testing.
// All other states are for automatic control/competition.
enum RobotState {
  MANUAL,
  STARTUP, 
  SETUP, READY,
  DRIVING, TURNING,
  PROBING,
  ACQUIRING, GRABBING,
  PLACING,
  RESTART
};
enum RobotState state;

// Struct that holds priority queues for different block types. Queue is an array that will
// pop from left to right until all values have been visited and index == size.
struct PriorityQueue {
  int index;
  int size;
  int queue[20];
};
struct PriorityQueue wheelQueue;
struct PriorityQueue batteryQueue;
struct PriorityQueue fanQueue;

// Block types
enum Block : char {
  WHEEL = 'w',
  BATTERY = 'b',
  FAN = 'f'
};

// Transmission variables
char rxChar;
int rxInt;

void setup() {
  state = STARTUP;
  debugMode = ON;

  // Set up Comms
  serialSetup();

  // Set up stepper motors
  stepperSetup();

  // Set up servo motors

  // Set up DC motors
  motorSetup();

  // Set up line sensor array
  arraySetup();
  
  // Set up distance sensors
  distanceSetup();

  // Set up electromagnet

  // Set up color detector
}

void loop() {
  switch (state) {
    case STARTUP: {
      checkManualControl();
    } break;

    case MANUAL: {
      manualOperations();
    } break;

    case SETUP: {

    } break;
    
    case READY: {

    } break;
    
    case DRIVING: {

    } break;
    
    case TURNING: {

    } break;
    
    case PROBING: {

    } break;
    
    case ACQUIRING: {

    } break;
    
    case GRABBING: {

    } break;
    
    case PLACING: {

    } break;
    
    case RESTART: {

    } break;
    
    default: {

    }
  }
}