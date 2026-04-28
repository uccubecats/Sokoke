# bmp_function

**Files:** `bmp_function.h` · `bmp_function.cpp`

## Overview

Thin wrapper around the [Adafruit BMP3XX](https://github.com/adafruit/Adafruit_BMP3XX) library that reads temperature and pressure from a BMP390 sensor and returns the results in a simple struct. Two independent BMP390 instances are used in the main application — one inside and one outside the enclosure — each on a different I2C address.

---

## Dependencies

- **Adafruit_BMP3XX** — Adafruit driver for the BMP3XX family of pressure sensors

---

## Data Types

### `BmpData`

| Field      | Type    | Description                                            |
|------------|---------|--------------------------------------------------------|
| `temp`     | `float` | Temperature in °C                                      |
| `pressure` | `float` | Barometric pressure in Pa                              |
| `success`  | `bool`  | `true` if the read succeeded; `false` otherwise        |

---

## Public API

#### `BmpData read_bmp(Adafruit_BMP3XX* bmp, bool bmp_alive)`

Performs a single BMP390 measurement and returns the result.

| Parameter   | Description                                                      |
|-------------|------------------------------------------------------------------|
| `bmp`       | Pointer to an initialized `Adafruit_BMP3XX` instance             |
| `bmp_alive` | Sensor alive flag; if `false`, returns `{0}` without reading     |

Returns a `BmpData` struct. If the sensor is not alive or `performReading()` fails, all fields are zero and `success` is `false`.

---

## Sensor Instances

Two BMP390 sensors are declared in `SensorInit.cpp` / `sensorInit.h` and initialized during `setup()`:

| Variable      | I2C Address | Role                          |
|---------------|-------------|-------------------------------|
| `bmp_inside`  | `0x76`      | Inside enclosure temp/pressure|
| `bmp_outside` | `0x77`      | Outside enclosure temp/pressure|

The corresponding alive flags are `bmp_inside_alive` and `bmp_outside_alive`.

---

## Usage Example

```cpp
BmpData inside = read_bmp(&bmp_inside, bmp_inside_alive);
if (inside.success) {
    writeDataToBuffer("InsBmpTemp",  inside.temp);
    writeDataToBuffer("InsBmpPress", inside.pressure);
}

BmpData outside = read_bmp(&bmp_outside, bmp_outside_alive);
if (outside.success) {
    writeDataToBuffer("OutBmpTemp",  outside.temp);
    writeDataToBuffer("OutBmpPress", outside.pressure);
}
```
