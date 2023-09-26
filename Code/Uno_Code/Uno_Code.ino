#include <SoftwareSerial.h>

#define rxPin 10
#define txPin 11

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
enum State state;

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
  delay(1000);
  
  Serial.println("Welcome");
  state = RECEIVING;
}

void loop() {
  switch (state) {
    case SENDING: {
      txChar = 0;
      txInt = 0;
      
      Serial.println("Waiting for data.");

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
  Serial.println("Please input a [char][int] into the serial monitor.");
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

      Serial.print("Char: ");
      Serial.println(txChar);

      gotChar = true;
    }
    else {
      while (Serial.available() > 0) {
        txInt = Serial.read() - 48 + 10*txInt;
      }

      Serial.print("Int: ");
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

      Serial.print("Received character: ");
      Serial.println(rxChar);
      Serial.print("Received Int: ");
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
          Serial.println("Manual control?");
          Serial.println("Input (y/n) and any number");

          state = GETTING;
        } break;

        case 1: {
          Serial.println("Manual mode entered. The following commands are available:");
          Serial.println("Drive forward [int] seconds: f/F [int]");
          Serial.println("Reverse [int] seconds: b/B [int]");
          Serial.println("Turn left [int] seconds: l/L [int]");
          Serial.println("Turn right [int] seconds: r/R [int]");
          Serial.println("Move turret [int] degrees: t/T [int] left: [0 90] right: [91 180]");
          Serial.println("Move arm [int] degrees: a/A [int] left: [0 90] right: [91 180]");
          Serial.println("Move wrist [int] degrees: w/W [int] left: [0 90] right: [91 180]");
          Serial.println("Move arm to vertical position [int]: z/Z [int] [0 1]");

          state = GETTING;
        } break;

        case 2: {
          Serial.println("Driving forward.");

        } break;

        case 3: {
          Serial.println("Driving in reverse.");

        } break;

        case 4: {
          Serial.println("Turning left.");

        } break;

        case 5: {
          Serial.println("Turning right.");

        } break;

        case 6: {
          Serial.println("Moving turret.");

        } break;

        case 7: {
          Serial.println("Moving arm.");

        } break;

        case 8: {
          Serial.println("Moving wrist.");

        } break;

        case 9: {
          Serial.println("Moving vertically");

        } break;
      }
    } break;

    case 'B': {
      switch(rxInt) {
        case 0: {
          Serial.println("Beginning autonomous processing.");
          Serial.println("Add block to chassis: [blocktype][position]");
          Serial.println("Block type can be w/W, b/B, or f/F");

          state = GETTING;

        } break;

        case 1: {
          Serial.println("Add additional blocks if desired:");

          state = GETTING;

        } break;
      }
    } break;

    case 'S': {
      switch(rxInt) {
        case 0: {
          Serial.println("Chassis set up. Awaiting START command: s/S 0");

          state = GETTING;
        } break;

        case 1: {
          Serial.println("Robot beginning autonomous operations.");

          state = GETTING;

        } break;
      }
    } break;

    case 'R': {
      switch(rxInt) {
        case 0: {
          Serial.println("RESTART command available: r/R 0");

          state = GETTING;

        } break;

        case 1: {
          Serial.println("RESTART initiated!");

        } break;
      }
    } break;

    default: {
      Serial.println("Invalid command received.");

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