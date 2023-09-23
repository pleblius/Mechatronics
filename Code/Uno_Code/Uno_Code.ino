#include <SoftwareSerial.h>

#define rxPin 10
#define txPin 11

SoftwareSerial mySerial(rxPin, txPin);

char txChar = 0;
int txInt = 0;

enum State {
  NO_INPUT,
  CHAR_INPUT
};
enum State state;

void setup() {
  state = NO_INPUT;

  Serial.begin(9600);
  mySerial.begin(9600);

  serialOutput();
}

void loop() {
  receiveTransmission();

  getUserInput();
}

void receiveTransmission() {
  char input;

  while (mySerial.available() > 0) {
    input = mySerial.read();

    Serial.print(input);
  }
}

void getUserInput() {
  delay(1000);
  if (Serial.available() > 0) {
    switch (state) {
      case NO_INPUT:
        txChar = Serial.read();

        Serial.print(txChar);
        
        state = CHAR_INPUT;
        serialOutput();

        break;
      case CHAR_INPUT:
        while (Serial.available() > 0) {
          txInt = Serial.read() - 48 + 10*txInt;
        }

        Serial.print(txInt);
        
        state = NO_INPUT;
        serialOutput();

        sendTransmission(txChar, txInt);
        txChar = 0;
        txInt = 0;

        break;
    }
  }
}

void serialOutput() {
  switch (state) {
    case NO_INPUT:
      Serial.print("\nChar input: ");
      break;
    case CHAR_INPUT:
      Serial.print("\nInt input: ");
      break;
  }
}

void sendTransmission(char ch, int i) {
  mySerial.write(255);
  mySerial.write(ch);
  mySerial.write(i);
}