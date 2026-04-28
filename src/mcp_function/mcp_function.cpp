#include "src/mcp_function/mcp_function.h"
#include "src/Sensors.h"

McpData read_mcp() {
    if (!mcp_alive) return {0};

    return (McpData){
        .temp_c = mcp.readTempC(),
        .temp_f = mcp.readTempF(),
        .success = true,
    };
}