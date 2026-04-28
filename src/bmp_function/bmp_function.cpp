#include "src/bmp_function/bmp_function.h"

BmpData read_bmp(Adafruit_BMP3XX *bmp, bool bmp_alive) {
    if (!bmp->performReading() || !bmp_alive) return {0};

    return (BmpData){
        .temp = bmp->temperature,
        .pressure = bmp->pressure,
        .success = true,
    };
}