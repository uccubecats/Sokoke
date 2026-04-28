#include "src/gyro_function/gyro_function.h"

#include "src/Sensors.h"

GyroData read_fxos_fxas_gyro() {
    if (!fxos_fxas_alive) return {0};

    sensors_event_t a, m, g;
    fxos.getEvent(&a, &m);
    fxas.getEvent(&g);

    // 1. Apply Mag Calibration
    float mx = m.magnetic.x - mag_offsets[0];
    float my = m.magnetic.y - mag_offsets[1];
    float mz = m.magnetic.z - mag_offsets[2];

    // 2. Update Filter (Note: Units must be Degrees/Sec for this library)
    // Converting Rad/s to Deg/s
    madgwick.update(g.gyro.x * 57.2958, g.gyro.y * 57.2958, g.gyro.z * 57.2958,
                  a.acceleration.x, a.acceleration.y, a.acceleration.z,
                  mx, my, mz);

    // 3. Get Orientation (Angles)
    float roll = madgwick.getRoll();
    float pitch = madgwick.getPitch();
    float yaw = madgwick.getYaw();

    // 4. Calculate Linear Acceleration (Position tracking precursor)
    // We must rotate the raw acceleration vector by the inverse of the orientation
    // to remove the 9.8 m/s^2 of gravity.

    // --- Output ---
    
    return (GyroData){
        .angle = {
            .roll = roll,
            .pitch = pitch,
            .yaw = yaw,
        },
        .linacc = {
            .x = a.acceleration.x,
            .y = a.acceleration.y,
            .z = a.acceleration.z - 9.81,
        },
        .success = true,
    };
    // Serial.print("ORIENTATION -> Roll: ");
    // Serial.print(roll);
    // Serial.print(" Pitch: ");
    // Serial.print(pitch);
    // Serial.print(" Yaw: ");
    // Serial.println(yaw);

    // Serial.print("LINEAR ACCEL (m/s^2) -> X: ");
    // Serial.println(lin_accel_x);
    // delay(10); // Maintain ~100Hz
}