# Sensors

**Files:** `Sensors.h`

## Overview

Defines the shared sensor status flags, PID tuning variables, and the `SensorDataType` enum used throughout the firmware to label CSV output, RockBLOCK entries, and runtime sensor state.

This header acts as the central naming and state contract between the sensor modules, logging code, and control logic.

---

## Dependencies

- **Arduino** - basic platform types and utilities via `Arduino.h`
- **FreeRTOS** - shared task/mutex types via `freertos/FreeRTOS.h`
- **SdFunction** - included for the logging buffer and RockBLOCK queue integration

---

## Exported Sensor Status

These globals indicate whether individual subsystems initialized successfully:

- `bmp_outside_alive`
- `bmp_inside_alive`
- `fxos_fxas_alive`
- `mcp_alive`
- `aht_alive`
- `ina_low_alive`
- `ina_high_alive`

They are typically set during startup by `SensorInit.cpp` and checked by the sensor read helpers before attempting to read data.

---

## PID Control Variables

These values are shared with the heater controller:

- `kp`
- `ki`
- `kd`
- `targetTemperature`
- `maxVal`

The PID controller module reads and updates these values to compute heater output.

---

## SensorDataType

`SensorDataType` is the canonical enum for all logged sensor channels.

### Order and layout

The enum order is important because it is used to:

- generate CSV headers in `SdFunction`
- map sensor names to RockBLOCK table entries
- keep runtime data aligned with the CSV column order

The final value, `SENSOR_COUNT`, must remain last so it can be used as the array length for sensor-name tables.

### Enum values

- `AhtTemperature`
- `AhtHumidity`
- `GyroRoll`
- `GyroPitch`
- `GyroYaw`
- `GyroLinAccX`
- `GyroLinAccY`
- `GyroLinAccZ`
- `InsBmpTemp`
- `InsBmpPress`
- `OutBmpTemp`
- `OutBmpPress`
- `McpTempF`
- `McpTempC`
- `LowInaBusVolt`
- `LowInaCurrent`
- `HighInaBusVolt`
- `HighInaCurrent`
- `SENSOR_COUNT`

---

## Helper Functions

#### `const char* get_sensor_name(SensorDataType type)`
Returns the string label for a given sensor enum value.

This is used when writing CSV rows and when matching runtime sensor names to the correct enum slot.

> **Important:** The function assumes `type` is within bounds of the `names` table. Callers should only pass valid `SensorDataType` values.

---

## Usage Example

```cpp
SensorDataType type = AhtTemperature;
const char *name = get_sensor_name(type);

if (aht_alive) {
    writeDataToBuffer(name, temperatureC);
}
```
