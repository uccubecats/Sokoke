#include <Arduino.h>
#include <BMP390.h>


const int pin21 = 21;
const int pin22 = 22;

BMP390 bmp(pin21, pin22);


void initBMP390() {
    if (!bmp.get_bmp_values().success) {
        while(1);
    }
}


bmp3_data Temp_Presure_Write_To_SD()
{
    return bmp.get_bmp_values();
}
