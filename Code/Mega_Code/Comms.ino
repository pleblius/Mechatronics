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
  Serial.println("Sending");
  Serial.println(txChar);
  Serial.println(txInt);
  Serial3.write(255);
  Serial3.write(txChar);
  Serial3.write(txInt);
}