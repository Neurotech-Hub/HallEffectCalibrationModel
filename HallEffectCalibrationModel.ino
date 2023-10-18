// by Matt Gaidica, PhD

#include "ADS1X15.h"

ADS1115 ADS(0x48);
ADS1115 ADS2(0x49);

float f, adcVals[8];

// const int nCal = 100;
int iCal = 0, i;
const int HALL_EN_GPIO = 13;

void setup() {
  Serial.begin(115200);
  digitalWrite(HALL_EN_GPIO,1);
  ADS.begin();
  ADS2.begin();
  ADS.setGain(0);
  ADS2.setGain(0);
  f = ADS.toVoltage(1);  // voltage factor
  Serial.println("\n\Initiated...");
}

void loop() {
  if (Serial.available()) {
    for (i = 0; i < 8; i++) {
      if (i < 4) {
        adcVals[i] = ADS.readADC(i) * f;
      } else {
        adcVals[i] = ADS2.readADC(i-4) * f;
      }
    }
    char incomingChar = Serial.read();
    if (incomingChar == 'r') {
      Serial.print(iCal);
      for (i = 0; i < 8; i++) {
        Serial.print('\t');
        Serial.print(adcVals[i], 3);
      }
      Serial.println("");
      iCal++;
    }
  }
  delay(50);
}