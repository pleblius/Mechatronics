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
  // Send manual prompt
  sendTransmission('M', 0);

  while (!receiveTransmission());
  
  switch (rxChar) {
    case 'y':
    case 'Y': {
      state = MANUAL;
      manualState = READY;

      sendTransmission('M', 1);

    } break;
    case 'n':
    case 'N': {
      sendTransmission('B', 0);
      
    } break;
  }
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
  Serial.println(str);
}

/*  Sends the given string to the serial comms devicce, for printout to the screen
 *  for debugging. Does not terminate the line.
 */
void debugPrint(char* str) {
  Serial.print(str);
}

/*  Sends the given float to the serial comms device, for printout to the screen
 *  for debugging. Ends with a newline character.
 */
void debugPrintln(float f) {
  Serial.println(f);
}

/*  Sends the given float to the serial comms device, for printout to the screen
 *  for debugging. Does not terminate the line.
 */
void debugPrint(float f) {
  Serial.print(f);
}

/*  Sends the given int to the serial comms device, for printout to the screen
 *  for debugging. Ends with a newline character.
 */
void debugPrintln(int f) {
  Serial.println(f);
}

/*  Sends the given int to the serial comms device, for printout to the screen
 *  for debugging. Does not terminate the line.
 */
void debugPrint(int f) {
  Serial.print(f);
}