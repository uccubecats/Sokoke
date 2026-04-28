#include "log_wrapper.h"
#include "../SdFunction/SdFunction.h"
#include <Arduino.h>
#include <stdarg.h>

LineoutReturn lineout(const char *output, bool debug_log,
                      bool trailing_newline) {
  LineoutReturn code = LineoutReturn::Success;

  if (trailing_newline) {
    if (Serial.println(output) == 0) {
      code = LineoutReturn::SerialFailure;
    }
  } else {
    if (Serial.print(output) == 0) {
      code = LineoutReturn::SerialFailure;
    }
  }

  if (trailing_newline) {
    if (SerialBT.println(output) == 0) {
      code = LineoutReturn::SerialFailure;
    }
  } else {
    if (SerialBT.print(output) == 0) {
      code = LineoutReturn::SerialFailure;
    }
  }

  if (debug_log) {
    bool owns_log_mutex = logMutex && xSemaphoreGetMutexHolder(logMutex) ==
                                          xTaskGetCurrentTaskHandle();
    if (!owns_log_mutex && logMutex) {
      if (xSemaphoreTake(logMutex, pdMS_TO_TICKS(5000)) != pdTRUE) {
        code = LineoutReturn::SDFailure;
        return code;
      }
    }

    FsFile debugFile = sd.open(DEBUG_LOG_FILE, O_RDWR | O_CREAT | O_APPEND);
    if (debugFile) {
      if (debugFile.print("[DEBUG ") == 0) {
        code = LineoutReturn::SDFailure;
      }
      if (debugFile.print(millis()) == 0) {
        code = LineoutReturn::SDFailure;
      }
      if (debugFile.print("] ") == 0) {
        code = LineoutReturn::SDFailure;
      }
      if (debugFile.print(output) == 0) {
        code = LineoutReturn::SDFailure;
      }
      if (trailing_newline && debugFile.print("\n") == 0) {
        code = LineoutReturn::SDFailure;
      }
      debugFile.close();
    } else {
      code = LineoutReturn::SDFailure;
    }

    if (!owns_log_mutex && logMutex) {
      xSemaphoreGive(logMutex);
    }
  }

  return code;
}

LineoutReturn lineoutPrintf(const char *format, ...) {
  char buffer[256];
  va_list args;
  va_start(args, format);
  vsnprintf(buffer, sizeof(buffer), format, args);
  va_end(args);

  return lineout(buffer, false, false);
}

LineoutReturn lineoutDebugPrintf(const char *format, ...) {
  char buffer[256];
  va_list args;
  va_start(args, format);
  vsnprintf(buffer, sizeof(buffer), format, args);
  va_end(args);

  return lineout(buffer, true, false);
}