
#include <Arduino.h>

/*
 * ESP32 MOSFET PWM Control via Serial Monitor
 * MOSFET: FQP30N06L
 */

// PWM Settings
const int pwmPin = 13;      // Connect to the Gate of the MOSFET
const int freq = 5000;      // 5 kHz frequency
const int ledChannel = 0;   // PWM channel (0-15)
const int resolution = 8;   // 8-bit resolution (0-255)

void setup() {
  Serial.begin(115200);
  
  // Configure LEDC PWM
  ledcSetup(ledChannel, freq, resolution);
  ledcAttachPin(pwmPin, ledChannel);
  
  Serial.println("--- MOSFET PWM Controller ---");
  Serial.println("Enter a duty cycle value (0-255):");
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
}