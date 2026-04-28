#pragma once

#ifndef LOG_WRAPPER_H
#define LOG_WRAPPER_H
#define DEBUG_LOG_FILE "/debug_log.txt"

#include "SdFat.h"
#include <BluetoothSerial.h>

extern BluetoothSerial SerialBT;

enum class LineoutReturn
{
    Success,
    SerialFailure,
    SerialBTFailure,
    SDFailure,
};

LineoutReturn lineout(const char *output,bool debug_log = true,  bool trailing_newline = true);
LineoutReturn lineoutPrintf(const char *format, ...);
LineoutReturn lineoutDebugPrintf(const char *format, ...);

#endif