#include <Arduino.h>
#include <Adafruit_INA228.h>
#include <algorithm>
#include <vector>
#include <iomanip>
#include <numeric>
#include <iostream>

Adafruit_INA228 ina228 = Adafruit_INA228();

std::vector<float> VbufferVec(256);
std::vector<float> AbufferVec(256);

void initializeINA228() {
if (!ina228.begin()) {
    Serial.println("Couldn't find INA228 chip");
    while (1)
      ;
  }
  Serial.println("Found INA228 chip");
  // set shunt resistance and max current
  ina228.setShunt(0.015, 10.0);

  ina228.setAveragingCount(INA228_COUNT_16);
  uint16_t counts[] = {1, 4, 16, 64, 128, 256, 512, 1024};
  Serial.print("Averaging counts: ");
  Serial.println(counts[ina228.getAveragingCount()]);

  // set the time over which to measure the current and bus voltage
  ina228.setVoltageConversionTime(INA228_TIME_150_us);
  Serial.print("Voltage conversion time: ");
  switch (ina228.getVoltageConversionTime()) {
  case INA228_TIME_50_us:
    Serial.print("50");
    break;
  case INA228_TIME_84_us:
    Serial.print("84");
    break;
  case INA228_TIME_150_us:
    Serial.print("150");
    break;
  case INA228_TIME_280_us:
    Serial.print("280");
    break;
  case INA228_TIME_540_us:
    Serial.print("540");
    break;
  case INA228_TIME_1052_us:
    Serial.print("1052");
    break;
  case INA228_TIME_2074_us:
    Serial.print("2074");
    break;
  case INA228_TIME_4120_us:
    Serial.print("4120");
    break;
  }
  Serial.println(" uS");

  ina228.setCurrentConversionTime(INA228_TIME_280_us);
  Serial.print("Current conversion time: ");
  switch (ina228.getCurrentConversionTime()) {
  case INA228_TIME_50_us:
    Serial.print("50");
    break;
  case INA228_TIME_84_us:
    Serial.print("84");
    break;
  case INA228_TIME_150_us:
    Serial.print("150");
    break;
  case INA228_TIME_280_us:
    Serial.print("280");
    break;
  case INA228_TIME_540_us:
    Serial.print("540");
    break;
  case INA228_TIME_1052_us:
    Serial.print("1052");
    break;
  case INA228_TIME_2074_us:
    Serial.print("2074");
    break;
  case INA228_TIME_4120_us:
    Serial.print("4120");
    break;
  }
  Serial.println(" uS");
}

void ReadINA228() {
    Serial.print("Bus Voltage: ");
    float busVoltage = ina228.getBusVoltage_V();
    Serial.print(busVoltage, 3);
    Serial.print(" V \nShunt Voltage: ");
    float current = ina228.getCurrent_mA();
    Serial.print(current, 3);
    Serial.println(" mA");
}
