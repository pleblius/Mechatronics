// Input pin
#define DSIN A15

// Calibration constants
float a = 3.7622;
float b = -0.8031;

// Cache inverses for efficiency
float a_inv = 1/a;
float b_inv = 1/b;

/* Sets up any distance sensor pins and initializes calibration values
 */
void distanceSetup() {
  pinMode(DSIN, INPUT);
}

/*  Gets the current distance reading in inches for the front-facing IR sensor. */
float getFrontDistance() {
  float voltage = getFrontVoltage();

  // Convert voltage to distance measurement
  float distance = pow(voltage*a_inv, b_inv);

  if (debugMode) {
    debugPrintln("Current Distance:");
    debugPrintln(distance);
    debugPrintln("");
  }

  return distance;
}

/*  Gets the voltage reading from the front-facing IR sensor.
 *  Uses IIR filter to reduce reading noise.
 *  NOTE: Call this every loop to ensure good data filtering. */
float getFrontVoltage() {
  static float prevVoltage = 0.0;
  int reading;
  float voltage;
  float distance;

  // Get voltage reading
  reading = analogRead(DSIN);

  // Map reading to voltage, 0V to 5V, and filter
  voltage = .001*map(reading, 0, 1023, 0, 5000);
  voltage = filterOutput(voltage, prevVoltage);
  
  // Cache voltage
  prevVoltage = voltage;

  return voltage;
}

/* Returns an IIR filtered output based on the previous and current floating-point readings
 */
float filterOutput(float curInput, float prevInput) {
  // Filter weight
  float alpha = 0.05;

  return alpha*curInput + (1.0 - alpha)*prevInput;
}