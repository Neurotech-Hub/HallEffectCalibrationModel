/* hall effect array ADC calibration routine
by Matt Gaidica, PhD
*/
#include "ADS1X15.h"
ADS1115 ADS(0x48);

const int hallSensorPins[] = {A0, A1, A2}; // Sensor pin numbers
const int numSensors = sizeof(hallSensorPins) / sizeof(hallSensorPins[0]);

struct DataPoint {
  float magnetPosition;
  int sensorReadings[numSensors];
};

const int numDataPoints = 5; // Number of data points for calibration
DataPoint dataPoints[numDataPoints];

const int numFeatures = numSensors; // Number of features (sensor readings)
float coefficients[numFeatures + 1]; // Coefficients including the intercept term
const float learningRate = 0.01; // Learning rate for gradient descent
const int numIterations = 1000; // Number of gradient descent iterations

void setup() {
  Serial.begin(9600);

  ADS.begin();
  
  for (int i = 0; i < numSensors; i++) {
    pinMode(hallSensorPins[i], INPUT);
  }
  
  // Collect data points
  for (int i = 0; i < numDataPoints; i++) {
    collectDataPoint(i);
  }

  // Perform linear regression
  performLinearRegression(coefficients);
}

void loop() {
  int rawSensorReadings[numSensors];
  
  for (int i = 0; i < numSensors; i++) {
    rawSensorReadings[i] = ADS.readADC(i);
  }

  // Estimate magnet position using linear model
  float estimatedPosition = estimateMagnetPosition(rawSensorReadings);

  // Print the estimated magnet position
  Serial.print("Estimated Position: ");
  Serial.println(estimatedPosition);

  delay(1000); // Delay for 1 second before reading again
}

void collectDataPoint(int index) {
  DataPoint dp;
  dp.magnetPosition = getUserInput("Enter magnet position for data point " + String(index) + ": ");
  
  for (int i = 0; i < numSensors; i++) {
    dp.sensorReadings[i] = ADS.readADC(i);
  }

  dataPoints[index] = dp;
}

float getUserInput(String prompt) {
  Serial.print(prompt);
  while (!Serial.available()) {
    // Wait for user input
  }
  return Serial.parseFloat();
}

void performLinearRegression(float coefficients[]) {
  // Initialize coefficients and perform gradient descent here
  // ...
}

float estimateMagnetPosition(int sensorReadings[]) {
  float estimatedPosition = coefficients[0]; // Intercept
  for (int i = 0; i < numFeatures; i++) {
    estimatedPosition += coefficients[i + 1] * sensorReadings[i];
  }
  return estimatedPosition;
}
