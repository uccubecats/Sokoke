# ina_function

**Files:** `ina_function.h` · `ina_function.cpp`

## Overview

Reads bus voltage and current from either an [INA228](https://www.ti.com/product/INA228) (high-precision, 20-bit) or an [INA219](https://www.ti.com/product/INA219) power monitor and returns the result in a unified struct. Two power monitors are used in the main application — one on the low-voltage rail (INA228) and one on the high-voltage rail (INA219).

---

## Dependencies

- **Adafruit_INA228** — Adafruit driver for the INA228 power monitor
- **Adafruit_INA219** — Adafruit driver for the INA219 power monitor

---

## Data Types

### `InaData`

| Field        | Type    | Description                                           |
|--------------|---------|-------------------------------------------------------|
| `busVoltage` | `float` | Bus voltage in V (bus + shunt voltage)                |
| `current`    | `float` | Current through the shunt resistor in mA              |
| `success`    | `bool`  | `true` if the read succeeded; `false` otherwise       |

---

## Public API

#### `InaData read_ina228(Adafruit_INA228* ina, bool ina_alive)`

Reads bus voltage and current from an INA228 sensor.

| Parameter   | Description                                                           |
|-------------|-----------------------------------------------------------------------|
| `ina`       | Pointer to an initialized `Adafruit_INA228` instance                  |
| `ina_alive` | Sensor alive flag; if `false`, returns `{0}` without reading          |

Bus voltage is computed as `getBusVoltage_V() + getShuntVoltage_mV() / 1000`.

---

#### `InaData read_ina219(Adafruit_INA219* ina, bool ina_alive)`

Reads bus voltage and current from an INA219 sensor.

| Parameter   | Description                                                           |
|-------------|-----------------------------------------------------------------------|
| `ina`       | Pointer to an initialized `Adafruit_INA219` instance                  |
| `ina_alive` | Sensor alive flag; if `false`, returns `{0}` without reading          |

Bus voltage is computed as `getBusVoltage_V() + getShuntVoltage_mV() / 1000`.

---

## Sensor Instances

Both sensors are declared in `SensorInit.cpp` / `sensorInit.h` and initialized during `setup()`:

| Variable   | Chip   | I2C Address | Rail         | Shunt Config          |
|------------|--------|-------------|--------------|------------------------|
| `ina_low`  | INA228 | `0x41`      | Low voltage  | 15 mΩ, 10 A max       |
| `ina_high` | INA219 | `0x40`      | High voltage | Default (100 mΩ)      |

The corresponding alive flags are `ina_low_alive` and `ina_high_alive`.

---

## Usage Example

```cpp
InaData low = read_ina228(&ina_low, ina_low_alive);
if (low.success) {
    writeDataToBuffer("LowInaBusVolt", low.busVoltage);
    writeDataToBuffer("LowInaCurrent", low.current);
}

InaData high = read_ina219(&ina_high, ina_high_alive);
if (high.success) {
    writeDataToBuffer("HighInaBusVolt", high.busVoltage);
    writeDataToBuffer("HighInaCurrent", high.current);
}
```
