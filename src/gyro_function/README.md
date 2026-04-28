# gyro_function

**Files:** `gyro_function.h` · `gyro_function.cpp`

## Overview

Reads orientation and linear acceleration from the paired [FXOS8700](https://www.nxp.com/products/sensors/motion-sensors/6-axis/6-axis-sensor-with-integrated-linear-accelerometer-and-magnetometer:FXOS8700CQ) accelerometer/magnetometer and [FXAS21002C](https://www.nxp.com/products/sensors/motion-sensors/6-axis/6-axis-sensor-with-integrated-gyroscope:FXAS21002C) gyroscope. Raw sensor data is fused through the [Madgwick AHRS](https://github.com/arduino-libraries/MadgwickAHRS) filter to produce roll, pitch, and yaw angles. Linear acceleration is estimated by subtracting gravity (9.81 m/s²) from the Z axis of the raw accelerometer reading.

---

## Dependencies

- **Adafruit_FXOS8700** — Adafruit driver for the FXOS8700 6-axis sensor
- **Adafruit_FXAS21002C** — Adafruit driver for the FXAS21002C gyroscope
- **MadgwickAHRS** — Madgwick sensor fusion filter library
- **Sensors.h** — `fxos_fxas_alive` alive flag

---

## Exported Globals

| Symbol         | Type                   | Description                                                         |
|----------------|------------------------|---------------------------------------------------------------------|
| `fxos`         | `Adafruit_FXOS8700`    | Accelerometer/magnetometer instance (defined in `SensorInit.cpp`)   |
| `fxas`         | `Adafruit_FXAS21002C`  | Gyroscope instance (defined in `SensorInit.cpp`)                    |
| `madgwick`     | `Madgwick`             | Madgwick filter instance (defined in `SensorInit.cpp`)              |
| `mag_offsets`  | `float[3]`             | Hard-iron calibration offsets for the magnetometer (X, Y, Z)        |

---

## Data Types

### `GyroData`

| Field            | Type    | Description                                                      |
|------------------|---------|------------------------------------------------------------------|
| `angle.roll`     | `float` | Roll angle in degrees                                            |
| `angle.pitch`    | `float` | Pitch angle in degrees                                           |
| `angle.yaw`      | `float` | Yaw angle in degrees                                             |
| `linacc.x`       | `float` | Linear acceleration X in m/s²  (raw accel X)                    |
| `linacc.y`       | `float` | Linear acceleration Y in m/s²  (raw accel Y)                    |
| `linacc.z`       | `float` | Linear acceleration Z in m/s²  (raw accel Z − 9.81)             |
| `success`        | `bool`  | `true` if the read succeeded; `false` otherwise                  |

---

## Public API

#### `GyroData read_fxos_fxas_gyro()`

Reads one sample from both sensors, applies magnetometer calibration offsets, updates the Madgwick filter, and returns orientation + linear acceleration.

Returns `{0}` (all zeros, `success = false`) if `fxos_fxas_alive` is `false`.

**Processing steps:**
1. Acquire accelerometer/magnetometer events from `fxos` and gyroscope event from `fxas`.
2. Apply hard-iron calibration: `mx = m.magnetic.x - mag_offsets[0]`, etc.
3. Update Madgwick filter. Gyroscope input is converted from rad/s to deg/s (× 57.2958).
4. Extract roll, pitch, yaw via `madgwick.getRoll/getPitch/getYaw()`.
5. Compute linear acceleration Z as `a.acceleration.z - 9.81`.

> **Note:** Linear acceleration on X and Y is currently the raw accelerometer value; full gravity-removal via orientation rotation is noted as a future improvement in the source.

---

## Usage Example

```cpp
GyroData gyro = read_fxos_fxas_gyro();
if (gyro.success) {
    writeDataToBuffer("GyroRoll",    gyro.angle.roll);
    writeDataToBuffer("GyroPitch",   gyro.angle.pitch);
    writeDataToBuffer("GyroYaw",     gyro.angle.yaw);
    writeDataToBuffer("GyroLinAccX", gyro.linacc.x);
    writeDataToBuffer("GyroLinAccY", gyro.linacc.y);
    writeDataToBuffer("GyroLinAccZ", gyro.linacc.z);
}
```
