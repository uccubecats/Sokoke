#include <Arduino.h>
#include <aht30Lib/driver_aht30.h>
#include <aht30Lib/driver_aht30_basic.h>
#include <ath30Function/ath30Function.h>

void setup() {
  Serial.begin(115200);
  initAht30();
}

void loop() {
  readAht30();
  delay(20);
}