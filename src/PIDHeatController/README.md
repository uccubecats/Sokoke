# PIDHeatController

**Files:** `PIDHeatController.h` · `PIDHeatController.cpp`

## Overview

A discrete-time PID (Proportional-Integral-Derivative) controller used to regulate a heater output. Given a target temperature, the current temperature, and the elapsed time since the last update, it computes a clamped output value suitable for driving a PWM heater channel (see [`PWMController/`](../PWMController/)). The derivative term uses an exponential low-pass filter to reduce noise.

---

## Configuration

The following variables are defined in `PIDHeatController.cpp` and can be updated at runtime (e.g. via Bluetooth commands):

| Variable             | Default | Description                                      |
|----------------------|---------|--------------------------------------------------|
| `kp`                 | `8.0`   | Proportional gain                                |
| `ki`                 | `0.0`   | Integral gain                                    |
| `kd`                 | `2.0`   | Derivative gain                                  |
| `targetTemperature`  | `35.0`  | Desired temperature in °C                        |
| `maxVal`             | `255.0` | Maximum output value (matches 8-bit PWM range)   |

The derivative low-pass filter constant is `DERIVATIVE_FILTER = 0.3` (higher = more responsive, less filtered).

---

## Dependencies

- **Sensors.h** — `extern` declarations for `kp`, `ki`, `kd`, `targetTemperature`, `maxVal`

---

## Public API

#### `float CalculatePID(float target, float current, float dt)`
Computes the PID output for one time step.

| Parameter | Description                                   |
|-----------|-----------------------------------------------|
| `target`  | Desired temperature (°C)                      |
| `current` | Current measured temperature (°C)             |
| `dt`      | Time elapsed since last call (seconds)        |

Returns a float clamped to `[0, maxVal]`. Returns `0.0` if `dt <= 0`.

**Internal state maintained across calls:**
- `integral` — accumulated error × dt, clamped to `[-maxVal/ki, maxVal/ki]`
- `last_error` — previous error for derivative calculation
- `filtered_derivative` — low-pass filtered derivative: `(1 - DERIVATIVE_FILTER) * prev + DERIVATIVE_FILTER * raw`

---

#### `void ResetPID()`
Resets all internal PID state (`integral`, `derivative`, `last_error`, `filtered_derivative`) to zero. Should be called at startup and whenever the controller is temporarily disabled (e.g. low-pressure event) to prevent integral windup from carrying over.

---

## Bluetooth Commands (handled in `writeCore()`)

The following commands can be sent over Bluetooth Serial to tune the controller at runtime:

| Command          | Example        | Effect                          |
|------------------|----------------|---------------------------------|
| `kp <value>`     | `kp 10.0`      | Updates proportional gain       |
| `ki <value>`     | `ki 0.5`       | Updates integral gain           |
| `kd <value>`     | `kd 1.5`       | Updates derivative gain         |
| `target <value>` | `target 30.0`  | Updates target temperature (°C) |

---

## Usage Example

```cpp
ResetPID();  // Call once at startup

// In a 1-second periodic task:
float pidOutput = CalculatePID(targetTemperature, currentTemp, 1.0f);
uint32_t duty = std::max(25u, static_cast<uint32_t>(pidOutput));
ledcWrite(heaterPin, duty);
```
