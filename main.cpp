#include <Arduino.h>
#include <aht30Lib/driver_aht30.h>
#include <aht30Lib/driver_aht30_basic.h>
#include <ath30Function/ath30Function.h>
#include <INA/INA.h>
#include <PIDHeatController/PIDHeatController.h>
#include <PWMController/PWMController.h>

uint64_t lastTime = 0;
float targetTemperature = 30.0f;

void setup() {
  Serial.begin(115200);
  initAht30();
  initializeINA228();
  PWMSetup();
}

void loop() {

  if (Serial.available() > 0) {
    // Read the input as an integer
    int dutyCycle = Serial.parseInt();

    // Constrain the value to 8-bit range (0-255)
    dutyCycle = constrain(dutyCycle, 0, 255);

    // Apply the PWM signal
    ledcWrite(ledChannel, dutyCycle);

    // Feedback
    Serial.print("Duty Cycle set to: ");
    Serial.print(dutyCycle);
    Serial.print(" (");
    Serial.print((dutyCycle / 255.0) * 100);
    Serial.println("%)");
    
    // Clear buffer
    while(Serial.available() > 0) Serial.read();
  }
  


  if (millis() - lastTime >= 1000) {
    ReadINA228();

    float temp = readAht30();
    float pidOutput = CalculatePID(targetTemperature, temp, 1.0f);
    Serial.print("PID Output: ");
    Serial.println(pidOutput);
    printAht30();

    Serial.println();
    lastTime = millis();
  }
  delay(50);
}