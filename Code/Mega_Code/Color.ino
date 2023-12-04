#define S0 42
#define S1 44
#define S2 46
#define S3 48
#define sensorOut 50

float redFreq = 0;
float blueFreq = 0;
float greenFreq = 0;

// Sensor read values
int redReading;
int greenReading;
int blueReading;
int clearReading;

// Sensor ranges for each color
int redLow, redHigh;
int blueLow, blueHigh;
int greenLow, greenHigh;
int clearLow, clearHigh;

/* Setps up the color sensor with its pre-calibrated values. */
void colorSetup() {
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(sensorOut, INPUT);

  digitalWrite(S0,HIGH);
  digitalWrite(S1,LOW);
}

bool isEmpty() {
  float redRange[2] = {200, 1000};
  float blueRange[2] = {300, 1000};
  float greenRange[2] = {300, 1000};

  return inRange(redFreq, redRange[0], redRange[1]) &&
    inRange(blueFreq, blueRange[0], blueRange[1]) &&
    inRange(greenFreq, greenRange[0], greenRange[1]);
}

bool isRed() {
  float redRange[2] = {50, 140};
  float blueRange[2] = {100, 400};
  float greenRange[2] = {120, 350};

  return inRange(redFreq, redRange[0], redRange[1]) &&
    inRange(blueFreq, blueRange[0], blueRange[1]) &&
    inRange(greenFreq, greenRange[0], greenRange[1]);
}

bool isBlue() {
  float redRange[2] = {150, 500};
  float blueRange[2] = {60, 200};
  float greenRange[2] = {110, 500};

  return inRange(redFreq, redRange[0], redRange[1]) &&
    inRange(blueFreq, blueRange[0], blueRange[1]) &&
    inRange(greenFreq, greenRange[0], greenRange[1]);  
}

bool isYellow() {
  float redRange[2] = {25, 80};
  float blueRange[2] = {50, 130};
  float greenRange[2] = {30, 100};

  return inRange(redFreq, redRange[0], redRange[1]) &&
    inRange(blueFreq, blueRange[0], blueRange[1]) &&
    inRange(greenFreq, greenRange[0], greenRange[1]);
}

bool inRange(float val, float min, float max) {
  return val > min && val < max;
}

void getColor() {
  // Setting red filtered photodiodes to be read
  digitalWrite(S2,LOW);
  digitalWrite(S3,LOW);
  // Reading the output frequency
  redFreq = pulseIn(sensorOut, LOW);

  // Setting Green filtered photodiodes to be read
  digitalWrite(S2,HIGH);
  digitalWrite(S3,HIGH);
  // Reading the output frequency
  greenFreq = pulseIn(sensorOut, LOW);

  // Setting Blue filtered photodiodes to be read
  digitalWrite(S2,LOW);
  digitalWrite(S3,HIGH);
  // Reading the output frequency
  blueFreq = pulseIn(sensorOut, LOW);
}