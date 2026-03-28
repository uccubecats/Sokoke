#include <Arduino.h>
#include <aht30Lib/driver_aht30.h>
#include <aht30Lib/driver_aht30_basic.h>
#include <vector>
#include <iomanip>
#include <numeric>
#include <iostream>
#include "ath30Function.h"

std::vector<float> TbufferVec(256);

float temperature;
uint8_t humidity;

void initAht30() {
    if (aht30_basic_init() != 0){
        SerialBT.println("aht30: init failed.");
    
        while (1);
    }
    else{
        SerialBT.println("aht30: init success.");
    }
}

float readAht30() {
  float temperature;
  uint8_t humidity;
  
  if (aht30_basic_read(&temperature, &humidity) != 0) {
    SerialBT.println("aht30: read failed.");
  }
  else {
    TbufferVec.push_back(temperature);
    return temperature;
  }
  return 0.0f;
}

void printAht30() {
  if (!TbufferVec.empty()) {
    SerialBT.print("Temperature: ");
    SerialBT.print(std::accumulate(TbufferVec.begin(), TbufferVec.end(), 0.0) / TbufferVec.size(), 2);
    SerialBT.println(" °C");
    TbufferVec.clear();
  }
}