#define SERVOPIN 4
Servo pressServo;

void servoSetup() {
  pressServo.attach(SERVOPIN);
  rotatePresser(0);
}

/*  Rotates the block-catching servo to the provided angle.
 */
void rotatePresser(int angle) {
  pressServo.write(angle);
}