#include <Arduino.h>
#include <aht30Lib/driver_aht30.h>
#include <aht30Lib/driver_aht30_basic.h>
#include <ath30Function/ath30Function.h>
#include <INA/INA.h>
#include <PIDHeatController/PIDHeatController.h>
#include <PWMController/PWMController.h>
#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

uint64_t lastTime = 0;
float targetTemperature = 30.0f;

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32_PID_Controller");
  initAht30();
  initializeINA228();
  PWMSetup();
}

void loop() {

  if (SerialBT.available() > 0) {
    // Read the input as an integer
    int dutyCycle = SerialBT.parseInt();

    // Constrain the value to 8-bit range (0-255)
    dutyCycle = constrain(dutyCycle, 0, 255);

    // Apply the PWM signal
    ledcWrite(ledChannel, dutyCycle);

    // Feedback
    SerialBT.print("Duty Cycle set to: ");
    SerialBT.print(dutyCycle);
    SerialBT.print(" (");
    SerialBT.print((dutyCycle / 255.0) * 100);
    SerialBT.println("%)");
    
    // Clear buffer
    while(SerialBT.available() > 0) SerialBT.read();
  }
  


  if (millis() - lastTime >= 1000) {
    ReadINA228();

    float temp = readAht30();
    float pidOutput = CalculatePID(targetTemperature, temp, 1.0f);
    SerialBT.print("PID Output: ");
    SerialBT.println(pidOutput);
    printAht30();

    SerialBT.println();
    lastTime = millis();
  }
  delay(50);
}