# PWMController

**Files:** `PWMController.h` · `PWMController.cpp`

## Overview

Configures the ESP32's LEDC (LED Control) peripheral to generate a PWM signal for driving a [FQP30N06L](https://www.onsemi.com/products/discrete-power-modules/mosfets/fqp30n06l) N-channel MOSFET. This is used to control a resistive heater or a cooling fan via duty cycle. Intended to be used alongside [`PIDHeatController`](../PIDHeatController/) which calculates the duty cycle value.

---

## Dependencies

- **log_wrapper** — `lineoutPrintf` for setup confirmation messages

---

## Public API

#### `void PWMSetup(int pwmPin, int freq, int resolution)`
Attaches `pwmPin` to the LEDC peripheral with the specified frequency and resolution using `ledcAttach()`, then prints the configuration over Serial and Bluetooth.

| Parameter    | Description                                                        |
|--------------|--------------------------------------------------------------------|
| `pwmPin`     | GPIO pin connected to the MOSFET gate                              |
| `freq`       | PWM carrier frequency in Hz                                        |
| `resolution` | Duty cycle resolution in bits (e.g. `8` → range 0–255)            |

After calling `PWMSetup()`, use `ledcWrite(pwmPin, value)` to update the duty cycle.

---

## Usage in the Application

The heater and fan are each initialized separately during `setup()`:

```cpp
PWMSetup(13, 5000, 8);  // Heater on GPIO 13
PWMSetup(12, 5000, 8);  // Fan on GPIO 12
```

| Channel | GPIO | Frequency | Resolution | Load   |
|---------|------|-----------|------------|--------|
| Heater  | 13   | 5000 Hz   | 8-bit      | Resistive heater |
| Fan     | 12   | 5000 Hz   | 8-bit      | Cooling fan      |

---

## Usage Example

```cpp
PWMSetup(13, 5000, 8);
PWMSetup(12, 5000, 8);

// In your control loop, driven by PID output:
float pidOutput = CalculatePID(targetTemperature, currentTemp, dt);
uint32_t duty = std::max(25u, static_cast<uint32_t>(pidOutput));
ledcWrite(13, duty);   // heater
ledcWrite(12, duty);   // fan
```
