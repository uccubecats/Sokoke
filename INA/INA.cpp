
#include <Arduino.h>
#include <Adafruit_INA228.h>
#include <algorithm>
#include <vector>
#include <iomanip>
#include <numeric>
#include <iostream>
#include "INA.h"

Adafruit_INA228 ina228 = Adafruit_INA228();

std::vector<float> VbufferVec(256);
std::vector<float> AbufferVec(256);

void initializeINA228() {
if (!ina228.begin()) {
    SerialBT.println("Couldn't find INA228 chip");
    while (1)
      ;
  }
  SerialBT.println("Found INA228 chip");
  // set shunt resistance and max current
  ina228.setShunt(0.015, 10.0);

  ina228.setAveragingCount(INA228_COUNT_16);
  uint16_t counts[] = {1, 4, 16, 64, 128, 256, 512, 1024};
  SerialBT.print("Averaging counts: ");
  SerialBT.println(counts[ina228.getAveragingCount()]);

  // set the time over which to measure the current and bus voltage
  ina228.setVoltageConversionTime(INA228_TIME_150_us);
  SerialBT.print("Voltage conversion time: ");
  switch (ina228.getVoltageConversionTime()) {
  case INA228_TIME_50_us:
    SerialBT.print("50");
    break;
  case INA228_TIME_84_us:
    SerialBT.print("84");
    break;
  case INA228_TIME_150_us:
    SerialBT.print("150");
    break;
  case INA228_TIME_280_us:
    SerialBT.print("280");
    break;
  case INA228_TIME_540_us:
    SerialBT.print("540");
    break;
  case INA228_TIME_1052_us:
    SerialBT.print("1052");
    break;
  case INA228_TIME_2074_us:
    SerialBT.print("2074");
    break;
  case INA228_TIME_4120_us:
    SerialBT.print("4120");
    break;
  }
  SerialBT.println(" uS");

  ina228.setCurrentConversionTime(INA228_TIME_280_us);
  SerialBT.print("Current conversion time: ");
  switch (ina228.getCurrentConversionTime()) {
  case INA228_TIME_50_us:
    SerialBT.print("50");
    break;
  case INA228_TIME_84_us:
    SerialBT.print("84");
    break;
  case INA228_TIME_150_us:
    SerialBT.print("150");
    break;
  case INA228_TIME_280_us:
    SerialBT.print("280");
    break;
  case INA228_TIME_540_us:
    SerialBT.print("540");
    break;
  case INA228_TIME_1052_us:
    SerialBT.print("1052");
    break;
  case INA228_TIME_2074_us:
    SerialBT.print("2074");
    break;
  case INA228_TIME_4120_us:
    SerialBT.print("4120");
    break;
  }
  SerialBT.println(" uS");
}

void ReadINA228() {
    SerialBT.print("Bus Voltage: ");
    float busVoltage = ina228.getBusVoltage_V();
    SerialBT.print(busVoltage, 3);
    SerialBT.print(" V \nShunt Voltage: ");
    float current = ina228.getCurrent_mA();
    SerialBT.print(current, 3);
    SerialBT.println(" mA");
}
