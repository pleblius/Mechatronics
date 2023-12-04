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

  while (mySerial.available() > 0) {
    mySerial.read();
  }
}

void loop() {
  switch (state) {
    case RECEIVING: {
      if (receiveTransmission()) {
        parseTransmission();
      }

    } break;

    case GETTING: {
      if (getUserInput()) {
        sendTransmission();

        state = RECEIVING;
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
      txInt = 0;
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

          state = GETTING;
        } break;
      }
    } break;

    case 'S': {
      switch(rxInt) {
        case 0: {
          Serial.println(F("Beginning startup. Use r/R at any time to restart operations."));
        } break;

        case 1: {
          Serial.println(F("Update blocks as necessary or begin competition."));
        } break;

        case 2: {
          Serial.println(F("Beginning competition! Initiate restart when desired with R command."));

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
        case 1: {
          Serial.println(F("Unknown command received."));
          
          state = GETTING;
        } break;

        case 99: {
          Serial.println(F("Unknown state reached."));

          state = GETTING;
        } break;
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