# SensorInit

**Files:** `sensorInit.h` · `SensorInit.cpp`

## Overview

Provides the startup and retry helpers that initialize the project hardware and shared globals.

The module owns the global sensor instances, initialization success flags, and retry-based setup functions for each sensor, the SD stack, the logging mutex, and the RockBLOCK subsystem.

---

## Dependencies

- **Arduino** - base platform support via `Arduino.h`
- **Adafruit sensor libraries** - `Adafruit_AHTX0`, `Adafruit_BMP3XX`, `Adafruit_FXAS21002C`, `Adafruit_FXOS8700`, `Adafruit_INA219`, `Adafruit_INA228`, `Adafruit_MCP9808`
- **MadgwickAHRS** - orientation filter used after the gyro and magnetometer initialize
- **BluetoothFunction** - startup support for Bluetooth logging
- **PIDHeatController** - shared heater control state
- **PWMController** - PWM output setup helpers
- **RockblockFunction** - RockBLOCK modem and queue initialization
- **SdFunction** - SD card initialization and logging buffer setup
- **Sensors** - shared sensor status flags and PID globals
- **log_wrapper** - startup messages through `lineout` and `lineoutPrintf`

---

## Retry Limit

`MAX_INIT_ATTEMPTS` is set to `10` and is used by every retry helper.

Each helper logs the current attempt, retries on failure, and returns `0` if the maximum number of attempts is exceeded.

---

## Exported Globals

### Sensor status flags

- `bmp_outside_alive`
- `bmp_inside_alive`
- `fxos_fxas_alive`
- `mcp_alive`
- `aht_alive`
- `ina_low_alive`
- `ina_high_alive`
- `pwm_fan_alive`
- `pwm_heater_alive`
- `rockblock_alive`

### Sensor and control instances

- `fxos` - `Adafruit_FXOS8700` accelerometer/magnetometer
- `fxas` - `Adafruit_FXAS21002C` gyroscope
- `madgwick` - `Madgwick` sensor fusion filter
- `mag_offsets` - 3-element magnetic calibration offset array
- `bmp_outside` - outside `Adafruit_BMP3XX` sensor
- `bmp_inside` - inside `Adafruit_BMP3XX` sensor
- `mcp` - `Adafruit_MCP9808` temperature sensor
- `aht` - `Adafruit_AHTX0` temperature/humidity sensor
- `ina_low` - `Adafruit_INA228` monitor
- `ina_high` - `Adafruit_INA219` monitor

---

## Public API

#### `int attempt_init_fxos8700(int current_attempt = 1)`
Initializes the FXOS8700 accelerometer/magnetometer. Retries until `MAX_INIT_ATTEMPTS` is reached.

#### `int attempt_init_fxas21002(int current_attempt = 1)`
Initializes the FXAS21002 gyroscope and starts the Madgwick filter at 100 Hz on success.

#### `int attempt_init_bmp390(Adafruit_BMP3XX *bmp, int address, int current_attempt = 1)`
Initializes a BMP390-compatible sensor on the provided I2C address.

#### `int attempt_init_mcp9808(int current_attempt = 1)`
Initializes the MCP9808 temperature sensor, sets resolution, and wakes the device.

#### `int attempt_init_aht30(int current_attempt = 1)`
Initializes the AHT30 temperature and humidity sensor.

#### `int attempt_init_mutex(int current_attempt = 1)`
Creates the FreeRTOS mutex used by the SD/logging pipeline.

#### `int attempt_init_sdreader(int current_attempt = 1)`
Initializes the SD card subsystem through `initSDCard()`.

#### `int attempt_init_ina228(Adafruit_INA228 *ina, int address, int current_attempt = 1)`
Initializes an INA228 monitor and applies the shunt configuration used by the project.

#### `int attempt_init_ina219(Adafruit_INA219 *ina, int address, int current_attempt = 1)`
Initializes an INA219 monitor on the given I2C address.

#### `int attempt_init_rockblock_buffer(int current_attempt = 1)`
Initializes the in-memory RockBLOCK buffer used by `SdFunction`.

#### `int attempt_init_rockblock(int current_attempt = 1)`
Initializes the Iridium modem through `initRockblock()`.

---

## Behavior Notes

- Each helper returns `1` on success and `0` on failure.
- The functions are intentionally small and recursive so startup code can retry without duplicating logging or error handling.
- `attempt_init_ina219()` reassigns the object before calling `begin()`, which ensures the configured address is used for the instance.

---

## Usage Example

```cpp
attempt_init_mutex();
attempt_init_sdreader();
attempt_init_aht30();
attempt_init_bmp390(&bmp_inside, 0x76);
attempt_init_rockblock();
```
