//https://github.com/RobTillaart/MS5611


#include "Arduino.h"
#include "MS5611.h"
#include <MS5607Function.h>


//  (TODO: check if 5607 has same breakout)
//  BREAKOUT  MS5611  aka  GY63 - see datasheet
//
//  SPI    I2C
//              +--------+
//  VCC    VCC  | o      |
//  GND    GND  | o      |
//         SCL  | o      |
//  SDI    SDA  | o      |
//  CSO         | o      |
//  SDO         | o L    |   L = led
//          PS  | o    O |   O = opening  PS = protocol select
//              +--------+
//
//  PS to VCC  ==>  I2C  (GY-63 board has internal pull up, so not needed)
//  PS to GND  ==>  SPI  (not supported in library)
//  CS to VCC  ==>  0x76
//  CS to GND  ==>  0x77


MS5607 MS5607(0x77);


void initMS5607()
{
  Wire.begin();
  if (!MS5607.begin())
    while (1);
  }
}

float getTemperature() const {return MS5607.getTemperature();} // Degrees C
float getPressure() const {return MS5607.getPressurePascal();} // Pascal
float getAltitude() const {return MS5607.getAltitude();} // Meters