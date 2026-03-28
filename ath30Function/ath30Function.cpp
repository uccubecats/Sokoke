#include <Arduino.h>
#include <aht30Lib/driver_aht30.h>
#include <aht30Lib/driver_aht30_basic.h>
#include <vector>
#include <iomanip>
#include <numeric>
#include <iostream>

std::vector<float> TbufferVec(256);

void initAht30() {
    if (aht30_basic_init() != 0){
        Serial.println("aht30: init failed.");
    
        while (1);
    }
    else{
        Serial.println("aht30: init success.");
    }
}

float readAht30() {
  float temperature;
  uint8_t humidity;
  
  if (aht30_basic_read(&temperature, &humidity) != 0) {
    Serial.println("aht30: read failed.");
  }
  else {
    TbufferVec.push_back(temperature);
    return temperature;
  }
  return 0.0f;
}

void printAht30() {
  if (!TbufferVec.empty()) {
    Serial.print("Temperature: ");
    Serial.print(std::accumulate(TbufferVec.begin(), TbufferVec.end(), 0.0) / TbufferVec.size(), 2);
    Serial.println(" °C");
    TbufferVec.clear();
  }
}