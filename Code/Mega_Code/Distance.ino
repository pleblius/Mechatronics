// Input pin
#define DSIN A0

// Calibration constants
float a;
float b;

// Cache inverses for efficiency
float a_inv;
float b_inv;

/* Sets up any distance sensor pins and initializes calibration values
 */
void distanceSetup() {
  pinMode(DSIN, INPUT);

  a = 1.0;
  b = 1.0;
  a_inv = 1.0/a;
  b_inv = 1.0/b;
}

/* Gets the current distance reading for the front-facing IR sensor
 */
float getFrontDistance() {
  static float prevVoltage = 0.0;
  int reading;
  float voltage;
  float distance;

  // Get voltage reading
  reading = analogRead(DSIN);

  // Map reading to voltage, 0V to 5V, and filter
  voltage = .001*map(reading, 0, 255, 0, 5000);
  voltage = filterOutput(voltage, prevVoltage);
  
  // Cache voltage
  prevVoltage = voltage;

  // Convert voltage to distance measurement
  distance = pow(voltage*a_inv, b_inv);

  if (debugMode) {
    debugPrintln("Current Distance:");
    debugPrintln(distance);
    debugPrintln("");
  }

  return distance;
}

/* Returns an IIR filtered output based on the previous and current floating-point readings
 */
float filterOutput(float curInput, float prevInput) {
  // Filter weight
  float alpha = 0.05;

  return alpha*curInput + (1.0 - alpha)*prevInput;
}