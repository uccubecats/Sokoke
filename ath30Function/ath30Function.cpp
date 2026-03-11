#include <Arduino.h>
#include <aht30Lib/driver_aht30.h>
#include <aht30Lib/driver_aht30_basic.h>

void initAht30() {
    if (aht30_basic_init() != 0){
        Serial.println("aht30: init failed.");
    
        while (1);
    }
    else{
        Serial.println("aht30: init success.");
    }
}

void readAht30() {
  float temperature;
  uint8_t humidity;
  
  if (aht30_basic_read(&temperature, &humidity) != 0) {
    Serial.println("aht30: read failed.");
  }
  else {
    Serial.printf("aht30: temperature is %.2fC.\n", temperature);
    Serial.printf("aht30: humidity is %d%%.\n", humidity);
  }
}