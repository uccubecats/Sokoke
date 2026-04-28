# aht30Function

**Files:** `aht30Function.h` · `aht30Function.cpp`

## Overview

Arduino C++ wrapper for the [AHT30](https://asairsensors.com/product/aht30/) temperature and humidity sensor, using the [Adafruit AHTX0](https://github.com/adafruit/Adafruit_AHTX0) library. Exposes a simple read function that returns temperature and relative humidity in a struct. Sensor initialization is handled by `attempt_init_aht30()` in [`SensorInit.cpp`](../SensorInit.cpp).

---

## Dependencies

- **Adafruit_AHTX0** — Adafruit driver for the AHT10/AHT20/AHT30 family of sensors
- **Sensors.h** — `aht_alive` sensor alive flag and `aht` sensor instance

---

## Exported Globals

| Symbol      | Type             | Description                                               |
|-------------|------------------|-----------------------------------------------------------|
| `aht`       | `Adafruit_AHTX0` | AHT30 sensor instance (defined in `SensorInit.cpp`)       |
| `aht_alive` | `bool`           | Set to `true` after successful initialization             |

---

## Data Types

### `AHT_Data_Return`

| Field         | Type    | Description                                         |
|---------------|---------|-----------------------------------------------------|
| `temperature` | `float` | Temperature in °C                                   |
| `humidity`    | `float` | Relative humidity in %                              |
| `success`     | `bool`  | `true` if the read succeeded; `false` otherwise     |

---

## Public API

#### `AHT_Data_Return readAht30()`
Reads temperature and humidity from the AHT30.

Returns `{0}` (all zeros, `success = false`) if:
- `aht_alive` is `false`, or
- either returned value is `NaN`.

---

## Initialization

The AHT30 is initialized during `setup()` via `attempt_init_aht30()` in `SensorInit.cpp`, which calls `aht.begin()` with up to `MAX_INIT_ATTEMPTS` retries. The result is stored in `aht_alive`.

To manually re-initialize at runtime (e.g. after a sensor fault), send the `initaht` command over Bluetooth Serial.

---

## Usage Example

```cpp
// Initialization is handled by SensorInit:
aht_alive = attempt_init_aht30();

// In sensor read loop:
AHT_Data_Return aht_data = readAht30();
if (aht_data.success) {
    writeDataToBuffer("AhtTemperature", aht_data.temperature);
    writeDataToBuffer("AhtHumidity",    aht_data.humidity);
}
```
