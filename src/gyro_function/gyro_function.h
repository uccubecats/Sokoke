#pragma once

#include <Adafruit_FXOS8700.h>
#include <Adafruit_FXAS21002C.h>
#include <MadgwickAHRS.h>

extern Adafruit_FXOS8700 fxos;
extern Adafruit_FXAS21002C fxas;
extern Madgwick madgwick;
extern float mag_offsets[3];

typedef struct {
    struct {
        float roll;
        float pitch;
        float yaw;
    } angle;
    struct {
        float x;
        float y;
        float z;
    } linacc;
    bool success;
} GyroData;

GyroData read_fxos_fxas_gyro();