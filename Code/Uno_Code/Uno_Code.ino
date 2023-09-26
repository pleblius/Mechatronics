#include <SoftwareSerial.h>

#define rxPin 10
#define txPin 11

SoftwareSerial mySerial(rxPin, txPin);

char txChar = 0;
int txInt = 0;

enum State {
  SENDING,
  RECEIVING,
  GETTING
};
enum State state;

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);

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
      int input = 0;

      if (mySerial.available() > 0) {
        input = mySerial.read();
      }

      if (input == 255) {
        printPrompt();

        state = GETTING;
      }
      else if (input == 254) {
        receiveTransmission();
      }
    } break;

    case GETTING: {
      if (getUserInput()) {
        sendTransmission(txChar, txInt);

        state = SENDING;
      }
    } break;
  }
}

/*  Opens every character in the wireless serial bus and prints it to the console.
 *  WARNING: This code blocks.
 */
void receiveTransmission() {
  char input;

  while (mySerial.available() > 0) {
    input = mySerial.read();

    Serial.print(input);
  }
}

/*  Prompts the user for data.
 */
void printPrompt() {
  Serial.println("Please input a [char][int] into the serial monitor.");
}

/*  Gets input from the user. Input needs to be in the form [char][int].
 *  Once data is acquired, it is transmitted to the robot.
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

/* Sends a serialized transmission in the order:
 *  255
 *  Character
 *  Integer
 */
void sendTransmission(char ch, int i) {
  mySerial.write(255);
  mySerial.write(ch);
  mySerial.write(i);
}