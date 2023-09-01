/* hall effect array ADC calibration routine
by Matt Gaidica, PhD
*/

// #include <LinearRegression.h>
#include "ADS1X15.h"

// LinearRegression lr = LinearRegression();
ADS1115 ADS(0x48);

const int numSensors = 3;

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
    Serial.print(rawSensorReadings[i]);
    Serial.print("\t");
  }
  Serial.println("");
  // Estimate magnet position using linear model
  float estimatedPosition = estimateMagnetPosition(rawSensorReadings);

  // Print the estimated magnet position
  Serial.print("Estimated Position: ");
  Serial.print(estimatedPosition);
  Serial.print(" | coeffs: ");
  for (int i = 0; i <= numFeatures; i++) {
    Serial.print(coefficients[i]);
    Serial.print("\t");
  }
  Serial.println("");

  delay(1000); // Delay for 1 second before reading again
}

void collectDataPoint(int index) {
  DataPoint dp;
  dp.magnetPosition = getUserInput("Enter magnet position for data point " + String(index) + ": ");
  Serial.print(dp.magnetPosition);
  Serial.print(" | ");
  for (int i = 0; i < numSensors; i++) {
    dp.sensorReadings[i] = ADS.readADC(i);
    Serial.print(dp.sensorReadings[i]);
    Serial.print("\t");
  }
  Serial.println("");
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
  for (int i = 0; i <= numFeatures; i++) {
    coefficients[i] = 0.0;
  }

  for (int iteration = 0; iteration < numIterations; iteration++) {
    float gradients[numFeatures + 1] = {0.0};

    for (int i = 0; i < numDataPoints; i++) {
      float prediction = coefficients[0];
      for (int j = 0; j < numFeatures; j++) {
        prediction += coefficients[j + 1] * dataPoints[i].sensorReadings[j];
      }
      
      float error = prediction - dataPoints[i].magnetPosition;

      gradients[0] += error;
      for (int j = 0; j < numFeatures; j++) {
        gradients[j + 1] += error * dataPoints[i].sensorReadings[j];
      }
    }

    for (int i = 0; i <= numFeatures; i++) {
      coefficients[i] -= learningRate * gradients[i] / numDataPoints;
    }
  }
}

float estimateMagnetPosition(int sensorReadings[]) {
  float estimatedPosition = coefficients[0];
  for (int i = 0; i < numFeatures; i++) {
    estimatedPosition += coefficients[i + 1] * sensorReadings[i];
  }
  return estimatedPosition;
}
