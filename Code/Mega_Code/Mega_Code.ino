#include <AccelStepper.h>
#include <MultiStepper.h>


/*********************** PIN DEFINITIONS **********************/

// Stepper Motors
#define ADIR 33
#define ZDIR 31
#define TDIR 29
#define ASTEP 27
#define ZSTEP 25
#define TSTEP 23

// DC Motors
#define IN1 9
#define IN2 10
#define IN3 11
#define IN4 12

#define CENA 45
#define CIN1 47
#define CIN2 49

// Line Sensor Array
#define LS1 22
#define LS2 24
#define LS3 26
#define LS4 28
#define LS5 30
#define LS6 32
#define LS7 34
#define LS8 36
#define LS9 38
#define LS10 40
#define LS11 42
#define LS12 44
#define LS13 46
#define LSIn 48

// Color Sensor
#define rgbSCL A4
#define rbgSDA A5

/************************ SETUP AND GLOBALS ********************/

// Sets the program's debug mode. Robot will print to comms bus, allowing
// debugging while under automatic control.
#define OFF false
#define ON true
bool debugMode;

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
  RESTART,
  ARM,
  TURRET,
  TOWER
};
enum RobotState state;
enum RobotState manualState;

// Struct that holds priority queues for different block types. Queue is an array that will
// pop from left to right until all values have been visited and index == size.
struct PriorityQueue {
  int index;
  int size;
  int queue[];
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

// Motor Directions
enum Direction : int {
  FORWARD = 1,
  REVERSE = -1,
  RIGHT = 2,
  LEFT = -2,
};

// Motor variables

float maxMotorSpeed;
float leftWheelSpeed;
float rightWheelSpeed;

int DC1Speed;
int DC2Speed;
int DC3Speed;
int DC4Speed;

// Steppers

float pi = 3.1416;

//Motor Block
float stepSize = 1.8;                       
float stepSizeRad = stepSize*pi/180.;       
float stepsPerRev = 360./stepSize;           

// Stepper constructurs - Motor mode (Driver = 2-pin), Step-pin, Dir-pin
AccelStepper armMotor = AccelStepper(AccelStepper::DRIVER, ASTEP, ADIR);
AccelStepper turretMotor = AccelStepper(AccelStepper::DRIVER, TSTEP, TDIR);  
AccelStepper zMotor = AccelStepper(AccelStepper::DRIVER, ZSTEP, ZDIR);

// Robot Dimensions

float wheelBase = 12.0;

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

  // Set up electromagnet

  // Set up color detector

  // DEBUG
  pinMode(13, OUTPUT);
  turretMotor.setMaxSpeed(1000);
  turretMotor.setSpeed(50);
  turretMotor.move(10000);
}

void loop() {
  // Time control variables - ms
  static unsigned long lastUpdate;
  float dt;
  
  dt = (millis() - lastUpdate)/1000.;
  lastUpdate = millis();

  switch (state) {
    case STARTUP: {
      checkManualControl();
    } break;

    case MANUAL: {
      manualOperations(dt);

      turretMotor.run();
      zMotor.run();
      armMotor.run();
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