

/*********************** PIN DEFINITIONS **********************/

// Stepper Motors
#define TSTEP 23
#define TDIR 25
#define ZSTEP 27
#define ZDIR 29
#define ASTEP 31
#define ADIR 33
#define WSTEP 35
#define WDIR 37

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
  STARTUP, READY,
  DRIVING, TURNING,
  PROBING,
  ACQUIRING, GRABBING,
  PLACING,
  RESTART
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

// Robot Dimensions

float wheelBase = 12.0;

// Transmission variables

char rxChar = 0;
int rxInt = 0;


void setup() {
  state = STARTUP;
  debugMode = ON;

  // Set up Comms
  serialSetup();

  // Set up stepper motors

  // Set up servo motors

  // Set up DC motors
  motorSetup();

  // Set up line sensor array

  // Set up electromagnet

  // Set up color detector
}

void loop() {
  // Time control variables - ms
  static unsigned long lastUpdate;
  unsigned int dt;
  
  dt = (millis() - lastUpdate);
  lastUpdate = millis();

  switch (state) {
    case MANUAL: {
      manualOperations(dt);
    } break;

    case STARTUP: {

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