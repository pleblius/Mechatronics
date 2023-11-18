/* Initializes the Serial communication channels on the Arduino Mega.
 * Serial controls USB comms with a PC, on pins 0 (RX) and 1 (TX).
 * Serial3 controls wireless communication with the UNO, on pins 15 (RX) and 14 (TX). */
void serialSetup() {
  Serial.begin(9600);
  Serial3.begin(9600);
}

/*  Receives a packet transmission from the Arduino Mega.
 *  Packet should be of form:
 *  255
 *  [char]
 *  [int]
 *  Returns true if a packet is received, false otherwise.
 */
bool receiveTransmission() {
  if (Serial3.available() > 2) {
    if (Serial3.read() == 255) {
      rxChar = Serial3.read();
      rxInt = Serial3.read();

      if (debugMode) {
        debugPrint("Received character: ");
        debugPrintln(rxChar);
        debugPrint("Received Int: ");
        debugPrintln(rxInt);
      }

      return true;
    }
  }

  return false;
}

/*  Sends a serialized transmission in the order:
 *  255
 *  Character
 *  Integer
 */
void sendTransmission(char txChar, int txInt) {
  Serial3.write(255);
  Serial3.write(txChar);
  Serial3.write(txInt);
}

/*  Sends the given string to the serial comms device, for printout to the screen
 *  for debugging. Ends with a newline character.
 */
void debugPrintln(char* str) {
  if (timer > dtTimer) {
    Serial.println(str);
  }
}

/*  Sends the given string to the serial comms devicce, for printout to the screen
 *  for debugging. Does not terminate the line.
 */
void debugPrint(char* str) {
  if (timer > dtTimer) {
    Serial.print(str);
  }
}

/*  Sends the given float to the serial comms device, for printout to the screen
 *  for debugging. Ends with a newline character.
 */
void debugPrintln(float f) {
  if (timer > dtTimer) {
    Serial.println(f);
  }
}

/*  Sends the given float to the serial comms device, for printout to the screen
 *  for debugging. Does not terminate the line.
 */
void debugPrint(float f) {  
  if (timer > dtTimer) {
    Serial.print(f);
  }
}

/*  Sends the given int to the serial comms device, for printout to the screen
 *  for debugging. Ends with a newline character.
 */
void debugPrintln(int f) {
  if (timer > dtTimer) {
    Serial.println(f);
  }
}

/*  Sends the given int to the serial comms device, for printout to the screen
 *  for debugging. Does not terminate the line.
 */
void debugPrint(int f) {
  if (timer > dtTimer) {
    Serial.println(f);
  }
}

/*  Sends the given int to the serial comms device, for printout to the screen
 *  for debugging. Ends with a newline character.
 */
void debugPrintln(long f) {
  if (timer > dtTimer) {
    Serial.println(f);
  }
}

/*  Sends the given int to the serial comms device, for printout to the screen
 *  for debugging. Does not terminate the line.
 */
void debugPrint(long f) {
  if (timer > dtTimer) {
    Serial.println(f);
  }
}

void forcedPrintln(char* str) {
  Serial.println(str);
}

void forcedPrint(char* str) {
  Serial.print(str);
}

void forcedPrintln(int f) {
  Serial.println(f);
}

void forcedPrint(int f) {
  Serial.print(f);
}

void forcedPrintln(float f) {
  Serial.println(f);
}

void forcedPrint(float f) {
  Serial.print(f);
}