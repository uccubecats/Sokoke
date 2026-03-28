
#include <Arduino.h>
#include <PWMController/PWMController.h>

/*
 * ESP32 MOSFET PWM Control via Serial Monitor
 * MOSFET: FQP30N06L
 */

void PWMSetup() {
  // Configure LEDC PWM
  ledcSetup(ledChannel, freq, resolution);
  ledcAttachPin(pwmPin, ledChannel);
  
  Serial.println("--- MOSFET PWM Controller ---");
  Serial.println("Enter a duty cycle value (0-255):");
}

