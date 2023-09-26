/* Initializes the Serial communication channels on the Arduino Mega.
 * Serial controls USB comms with a PC, on pins 0 (RX) and 1 (TX).
 * Serial3 controls wireless communication with the UNO, on pins 15 (RX) and 14 (TX). */
void serialSetup() {
  Serial.begin(9600);
  Serial3.begin(9600);
  delay(1000);

  // This code deliberately blocks while waiting for a response.
  checkManualControl();
}

/*  Gets user input if manual control is necessary. If so, enables it.
 *  WARNING: This code blocks.
 */
void checkManualControl() {
  debugPrintln("Manual Control Mode? (y/n)");
  delay(1000);
  sendGetSignal();

  char xCh;
  int xInt;

  while (!wirelessRead(xCh, xInt));
  
  switch (xCh) {
    case 'y':
    case 'Y': {
      state = MANUAL;
      manualState = READY;

      debugPrintln("Entering manual control.");
      delay(1000);
      debugPrintln("Please enter input: [char][int]");
      delay(1000);
    } break;
    case 'n':
    case 'N': {
      debugPrintln("Remaining in automatic control.");
      delay(1000);
      debugPrintln("Please enter input: [char][int]");
      delay(1000);
    } break;
  }
}

/*  Receives wireless communication via port Serial3 from the Arduino Uno. Communication is done in packets of three bytes:
 *  Byte 1: The number 255 (indicates packet beginning).
 *  Byte 2: A character, can be either 'b' (battery), 'w' (wheel), 'f' (fan), 'R' (Restart), or 'S' (Start).
 *  Byte 3: An integer, corresponding to a position or an empty placeholder value.
 *  Takes in two pointers to pass back to calling function for operational parsing.
 */
bool wirelessRead(char &ch, int &i) {
  // Check wireless buffer
  if (Serial3.available() > 2) {
    if (Serial3.read() == 255) {
      // Char value, should be 'b', 'w', 'f', 'R', or 'S'
      ch = Serial3.read();
      // Int value
      i = Serial3.read();

      if (debugMode) {
        debugPrint("Received characters: ");
        delay(100);
        debugPrint(ch);
        delay(100);
        debugPrint(" ");
        delay(100);
        debugPrintln(i + 48);
        delay(100);
      }

      return true;
    }
  }

  return false;
}

/*  Sends the given string to the serial comms device, for printout to the screen
 *  for debugging. Ends with a newline character.
 */
void debugPrintln(char* str) {
  Serial3.write(254);
  Serial3.println(str);
}

/*  Sends the given string to the serial comms devicce, for printout to the screen
 *  for debugging. Does not terminate the line.
 */
void debugPrint(char* str) {
  Serial3.write(254);
  Serial3.print(str);
}

/*  Sends the given float to the serial comms device, for printout to the screen
 *  for debugging. Ends with a newline character.
 */
void debugPrintln(float f) {
  Serial3.write(254);
  Serial3.println(f);
}

/*  Sends the given float to the serial comms device, for printout to the screen
 *  for debugging. Does not terminate the line.
 */
void debugPrint(float f) {
  Serial3.write(254);
  Serial3.print(f);
}

/*  Sends the given int to the serial comms device, for printout to the screen
 *  for debugging. Ends with a newline character.
 */
void debugPrintln(int f) {
  Serial3.write(254);
  Serial3.println(f);
}

/*  Sends the given int to the serial comms device, for printout to the screen
 *  for debugging. Does not terminate the line.
 */
void debugPrint(int f) {
  Serial3.write(254);
  Serial3.print(f);
}

/*  Sends a signal to the Uno to get the next command from the user.
 */
void sendGetSignal() {
  Serial3.write(255);
}