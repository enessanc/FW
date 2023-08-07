#pragma once
#include "common/mavlink.h"
#include <chrono>

namespace FW
{
    struct System
    {
        int system_id;
        int component_id;
        MAV_TYPE vehicle_id;
        MAV_AUTOPILOT autopilot_id;
        std::chrono::nanoseconds initial_sample_rtt;
    };
}
