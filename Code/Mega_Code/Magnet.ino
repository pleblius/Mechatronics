#define MAGNETON 38
#define MAGNETOFF 40

/*  Sets the magnet control pin to output mode. */
void magnetSetup() {
  pinMode(MAGNETON, OUTPUT);
  pinMode(MAGNETOFF, OUTPUT);

  digitalWrite(MAGNETON, LOW);
  digitalWrite(MAGNETOFF, LOW);
}

/*  Turns on the electromagnet. */
void activateMagnet() {
  digitalWrite(MAGNETON, HIGH);
  delay(3);
  digitalWrite(MAGNETON, LOW);
}

/*  Turns off the electromagnet. */
void deactivateMagnet() {
  digitalWrite(MAGNETOFF, HIGH);
  delay(3);
  digitalWrite(MAGNETOFF, LOW);
}