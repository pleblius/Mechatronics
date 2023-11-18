#define MAGNETPIN 52

/*  Sets the magnet control pin to output mode. */
void magnetSetup() {
  pinMode(MAGNETPIN, OUTPUT);
  deactivateMagnet();
}

/*  Turns on the electromagnet. */
void activateMagnet() {
  digitalWrite(MAGNETPIN, HIGH);
}

/*  Turns off the electromagnet. */
void deactivateMagnet() {
  digitalWrite(MAGNETPIN, LOW);
}