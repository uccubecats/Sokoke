#pragma once

#include <Adafruit_BMP3XX.h>

typedef struct {
    float temp;
    float pressure;
    bool success;
} BmpData;

BmpData read_bmp(Adafruit_BMP3XX *bmp, bool bmp_alive);