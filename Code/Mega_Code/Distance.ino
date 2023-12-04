// Input pin
#define REARIN A1
#define FRONTIN A0

// Calibration constants
// Front
float a1 = 3.3974;
float b1 = -0.8233;
// Back
float a2 = 1.5893;
float b2 = -0.9002;

// Cache inverses for efficiency
float a1_inv = 1/a1;
float b1_inv = 1/b1;

float a2_inv = 1/a2;
float b2_inv = 1/b2;

/* Sets up any distance sensor pins and initializes calibration values
 */
void distanceSetup() {
  pinMode(REARIN, INPUT);
  pinMode(FRONTIN, INPUT);
}

/*  Gets the current distance reading in inches for the front-facing IR sensor. */
float getFrontDistance() {
  float voltage = getFrontVoltage();

  // Convert voltage to distance measurement
  float distance = pow(voltage*a1_inv, b1_inv);

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
  reading = analogRead(FRONTIN);

  // Map reading to voltage, 0V to 5V, and filter
  voltage = .001*map(reading, 0, 1023, 0, 5000);
  voltage = filterOutput(voltage, prevVoltage);
  
  // Cache voltage
  prevVoltage = voltage;

  return voltage;
}

/*  Gets the current distance reading in inches for the rear-facing IR sensor. */
float getRearDistance() {
  float voltage = getRearVoltage();

  // Convert voltage to distance measurement
  float distance = pow(voltage*a2_inv, b2_inv);

  return distance;
}

/*  Gets the voltage reading from the front-facing IR sensor.
 *  Uses IIR filter to reduce reading noise.
 *  NOTE: Call this every loop to ensure good data filtering. */
float getRearVoltage() {
  static float prevVoltage = 0.0;
  int reading;
  float voltage;
  float distance;

  // Get voltage reading
  reading = analogRead(REARIN);

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
  float alpha = 0.02;

  return alpha*curInput + (1.0 - alpha)*prevInput;
}