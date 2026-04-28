#include <Arduino.h>

#include <Adafruit_AHTX0.h>
#include "src/aht30Function/aht30Function.h"
#include "src/Sensors.h"

AHT_Data_Return readAht30() {
  if (!aht_alive) return {0};
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity,
               &temp);
  if (isnan(humidity.relative_humidity) || isnan(temp.temperature)) return {0};

  return (AHT_Data_Return){
    .temperature = temp.temperature,
    .humidity = humidity.relative_humidity,
    .success = true,
  };
}