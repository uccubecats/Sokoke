#pragma once

#include <Adafruit_MCP9808.h>

extern Adafruit_MCP9808 mcp;

typedef struct {
    float temp_c;
    float temp_f;
    bool success;
} McpData;

McpData read_mcp();