/* Initializes the Serial communication channels on the Arduino Mega.
 * Serial controls USB comms with a PC, on pins 0 (RX) and 1 (TX).
 * Serial3 controls wireless communication with the UNO, on pins 15 (RX) and 14 (TX). */
void serialSetup() {
  Serial.begin(9600);

  Serial3.begin(9600);

  // This code deliberately blocks while waiting for a response.
  checkManualControl();
}

/*  Gets user input if manual control is necessary. If so, enables it.
 */
void checkManualControl() {
  debugPrintln("Manual Control Mode? (y/n)");

  while (Serial3.available() < 1);
  char input = Serial3.read();

  switch (input) {
    case 'y':
    case 'Y':
      state = MANUAL;
      manualState = READY;
      debugPrintln("Entering manual control.");
      debugPrintln("Please enter input: [char] [int]");
      break;
    case 'n':
    case 'N':
      break;
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
        debugPrint(ch);
        debugPrint(" ");
        debugPrintln(i + 48);
      }

      return true;
    }
  }

  return false;
}

/*  Sends the given string to the serial comms device, for printout to the screen
 *  for debugging. Ends with a newline character.
 */
void debugPrintln(char str[]) {
  Serial3.println(str);
}

/*  Sends the given string to the serial comms devicce, for printout to the screen
 *  for debugging. Does not terminate the line.
 */
void debugPrint(char str[]) {
  Serial3.print(str);
}

/*  Sends the given float to the serial comms device, for printout to the screen
 *  for debugging. Ends with a newline character.
 */
void debugPrintln(float f) {
  Serial3.println(f);
}

/*  Sends the given float to the serial comms device, for printout to the screen
 *  for debugging. Does not terminate the line.
 */
void debugPrint(float f) {
  Serial3.print(f);
}