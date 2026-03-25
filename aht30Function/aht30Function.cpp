#include <Arduino.h>
#include <aht30Lib/driver_aht30.h>
#include <aht30Lib/driver_aht30_basic.h>

bool initAht30() {
    if (aht30_basic_init() != 0){
        Serial.println("aht30: init failed.");
        return false; 
    }
    else{
        Serial.println("aht30: init success.");
        return true;
    }
}

std::tuple<float, uint8_t, bool> readAht30() {
  float temperature;
  uint8_t humidity;
  
  if (aht30_basic_read(&temperature, &humidity) != 0) {
    Serial.println("aht30: read failed.");
    return std::make_tuple(0.0f, 0, false);
  }
  // else {
  //   Serial.printf("aht30: temperature is %.2fC.\n", temperature);
  //   Serial.printf("aht30: humidity is %d%%.\n", humidity);
  // }
  return std::make_tuple(temperature, humidity, true);
}