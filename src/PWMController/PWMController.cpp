
#include <Arduino.h>
#include "src/PWMController/PWMController.h"
#include "src/log_wrapper/log_wrapper.h"

void PWMSetup(int pwmPin, int freq, int resolution)
{
  ledcAttach(pwmPin, freq, resolution);
  lineoutPrintf("PWM Setup: Pin %d, Frequency %d Hz, Resolution %d bits\n", pwmPin, freq, resolution);
}
