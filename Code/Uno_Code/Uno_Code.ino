#include <SoftwareSerial.h>

#define rxPin 2
#define txPin 3

SoftwareSerial mySerial(rxPin, txPin);

char txChar = 0;
int txInt = 0;
char rxChar = 0;
int rxInt = 0;

enum State {
  SENDING,
  RECEIVING,
  GETTING
};
State state;

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
  delay(1000);
  
  Serial.println(F("Welcome"));

  state = RECEIVING;
}

void loop() {
  switch (state) {
    case SENDING: {
      txChar = 0;
      txInt = 0;
      
      Serial.println(F("Waiting for data."));

      state = RECEIVING;
    } break;

    case RECEIVING: {
      if (receiveTransmission()) {
        parseTransmission();
      }

    } break;

    case GETTING: {
      if (getUserInput()) {
        sendTransmission();

        state = SENDING;
      }

    } break;
  }
}

/*  Prompts the user for data.
 */
void printPrompt() {
  Serial.println(F("Please input a [char][int] into the serial monitor."));
}

/*  Gets input from the user. Input needs to be in the form [char][int].
 *  Once data is acquired, it is transmitted to the robot.
 *  Returns true if both user inputs have been acquired, false otherwise.
 */
bool getUserInput() {
  static bool gotChar = false;

  if (Serial.available() > 0) {
    if (!gotChar) {
      txChar = Serial.read();

      Serial.print(F("Char: "));
      Serial.println(txChar);

      gotChar = true;
    }
    else {
      delay(500);

      while (Serial.available() > 0) {
        txInt = Serial.read() - 48 + 10*txInt;
      }

      Serial.print(F("Int: "));
      Serial.println(txInt);

      gotChar = false;
      return true;
    }
  }

  return false;
}

/*  Receives a packet transmission from the Arduino Mega.
 *  Packet should be of form:
 *  255
 *  [char]
 *  [int]
 *  Returns true if a packet is received, false otherwise.
 */
bool receiveTransmission() {
  if (mySerial.available() > 2) {
    if (mySerial.read() == 255) {
      rxChar = mySerial.read();
      rxInt = mySerial.read();

      Serial.print(F("Received character: "));
      Serial.println(rxChar);
      Serial.print(F("Received Int: "));
      Serial.println(rxInt);

      return true;
    }
  }

  return false;
}

/*  Parses the received transmission to display data for the user.
 *  Conditions to parse:
 *    M: Manual
 *      0: Manual prompt
 *      1: Available manual commands
 *      2: Driving Forward
 *      3: Reversing
 *      4: Turning left
 *      5: Turning right
 *      6: Moving turret
 *      7: Moving arm
 *      8: Moving wrist
 *      9: Moving vertically
 *    B: Blocks
 *      0: Add first block to chassis
 *      1: Add additional blocks to chassis
 *    S: Start
 *      0: Start prompt
 *      1: Starting
 *    R: Restart
 *      0: Restart prompt
 *      1: Restarting
 */
void parseTransmission() {
  switch(rxChar) {
    case 'M': {
      switch(rxInt) {
        case 0: {
          Serial.println(F("Manual control?"));
          Serial.println(F("Input (y/n) and any number"));

          state = GETTING;
        } break;

        case 100: {
          Serial.println(F(""));
          Serial.println(F("Manual mode entered. The following commands are available:"));
          Serial.println(F("Line-following until [int] distance: l/L [int]"));
          Serial.println(F("Drive via dead-reckoning: d/D [dist]"));
          Serial.println(F("Rotate in place [int] degrees: r/R [angle]"));
          Serial.println(F("Move turret [int] degrees: t/T [int] left: [0 90] right: [91 180]"));
          Serial.println(F("Move arm [int] degrees: a/A [int] left: [0 90] right: [91 180]"));
          Serial.println(F("Move arm to vertical position [int]: z/Z [int] [0 1]"));
          Serial.println(F("Move to (x,y,z) position: x/X [int]"));
          Serial.println(F("Begin autonomous block loading program: g/G [int]"));
          Serial.println(F("Begin autonomous block placing program: p/P"));
          Serial.println(F(""));

          state = GETTING;
        } break;

        case 1: {
          Serial.println(F(""));
          Serial.println(F("New Command:"));

          state = GETTING;
        } break;

        case 2: {
          Serial.println(F(""));
          Serial.println(F("Driving. Input direction [f/F/r/R] and speed (in/s) [int]."));

          state = GETTING;
        } break;

        case 3: {
          Serial.println(F(""));
          Serial.println(F("Rotating. Input direction [r/R/l/L] and speed (deg/s) [int]"));

          state = GETTING;
        } break;

        case 4: {
          Serial.println(F(""));
          Serial.println(F("Turning. Input direction [r/R/l/L] and speed (in/s) [int]"));

          state = GETTING;
        } break;

        case 5: {
          Serial.println(F(""));
          Serial.println(F("Line Following."));

        } break;

        case 6: {
          Serial.println(F(""));
          Serial.println(F("Moving Turret."));

        } break;

        case 7: {
          Serial.println(F(""));
          Serial.println(F("Moving Arm."));

        } break;

        case 8: {
          Serial.println(F(""));
          Serial.println(F("Moving vertically"));

        } break;

        case 9: {
          Serial.println(F(""));
          Serial.println(F("Next coordinate: "));
          state = GETTING;
        } break;

        case 10: {
          Serial.println(F(""));
          Serial.println(F("Moving to specified coordinates."));
        } break;

        case 11: {
          Serial.println(F(""));
          Serial.println(F("Running autonomous block-loading program."));
        } break;

        case 12: {
          Serial.println(F(""));
          Serial.println(F("Running autonomous block-placing program."));
        } break;
      }
    } break;

    case 'B': {
      switch(rxInt) {
        case 0: {
          Serial.println(F("Beginning autonomous processing."));
          Serial.println(F("Add block to chassis: [blocktype][position]"));
          Serial.println(F("Block type can be w/W, b/B, or f/F."));
          Serial.println(F("Block position can be 0-19."));

          state = GETTING;
        } break;

        case 1: {
          Serial.println(F("Add additional blocks if desired:"));

          state = GETTING;
        } break;

        case 9: {
          Serial.println(F("Invalid Block Type! Valid types are w/W, b/B, and f/F."));
          
          state = GETTING;
        } break;

        case 19: {
          Serial.println(F("Invalid Block Number! Valid locations are 0-19."));
        }
      }
    } break;

    case 'S': {
      switch(rxInt) {
        case 0: {
          Serial.println(F("Beginning startup. Use r/R at any time to restart operations."));
        } break;

        case 1: {
          Serial.println(F("Robot beginning autonomous operations."));

          state = GETTING;
        } break;
      }
    } break;

    case 'R': {
      switch(rxInt) {
        case 0: {
          Serial.println(F("Beginning restart."));
        } break;

        case 1: {
          Serial.println(F("Restart finished."));
        } break;
      }
    } break;

    case 'X': {
      switch(rxInt) {
        case 99: {
          Serial.println(F("Unknown state reached."));

          state = GETTING;
        }
      }
    } break;

    default: {
      Serial.println(F("Invalid command received."));

      state = GETTING;
      
    } break;
  }
}

/*  Sends a serialized transmission in the order:
 *  255
 *  Character
 *  Integer
 */
void sendTransmission() {
  mySerial.write(255);
  mySerial.write(txChar);
  mySerial.write(txInt);
}