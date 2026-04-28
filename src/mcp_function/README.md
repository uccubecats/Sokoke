# mcp_function

**Files:** `mcp_function.h` · `mcp_function.cpp`

## Overview

Thin wrapper around the [Adafruit MCP9808](https://github.com/adafruit/Adafruit_MCP9808_Library) library that reads the current temperature from an MCP9808 precision temperature sensor and returns it in both Celsius and Fahrenheit.

---

## Dependencies

- **Adafruit_MCP9808** — Adafruit driver for the MCP9808 temperature sensor
- **Sensors.h** — `mcp_alive` sensor alive flag

---

## Exported Globals

| Symbol | Type              | Description                                              |
|--------|-------------------|----------------------------------------------------------|
| `mcp`  | `Adafruit_MCP9808`| MCP9808 sensor instance (defined in `SensorInit.cpp`)    |

---

## Data Types

### `McpData`

| Field    | Type    | Description                                      |
|----------|---------|--------------------------------------------------|
| `temp_c` | `float` | Temperature in °C                                |
| `temp_f` | `float` | Temperature in °F                                |
| `success`| `bool`  | `true` if the read succeeded; `false` otherwise  |

---

## Public API

#### `McpData read_mcp()`

Reads the current temperature from the MCP9808 and returns it in both units.

Returns `{0}` (all zeros, `success = false`) if `mcp_alive` is `false`.

The sensor is initialized in `SensorInit.cpp` at I2C address `0x18` with resolution set to mode 3 (maximum precision, ±0.0625 °C) and woken from sleep.

---

## Usage Example

```cpp
McpData mcp_data = read_mcp();
if (mcp_data.success) {
    writeDataToBuffer("McpTempC", mcp_data.temp_c);
    writeDataToBuffer("McpTempF", mcp_data.temp_f);
}
```
