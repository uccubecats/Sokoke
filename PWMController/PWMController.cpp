
#include <Arduino.h>
#include <PWMController/PWMController.h>
#include <BluetoothFunction/BluetoothFunction.h>

/*
 * ESP32 MOSFET PWM Control via Serial Monitor
 * MOSFET: FQP30N06L
 */

void PWMSetup() {
  // Configure LEDC PWM
  ledcSetup(ledChannel, freq, resolution);
  ledcAttachPin(pwmPin, ledChannel);
  
  SerialBT.println("--- MOSFET PWM Controller ---");
  SerialBT.println("Enter a duty cycle value (0-255):");
}

