#pragma once
#include "common/mavlink.h"

struct SystemInfo
{
    int system_id;
    int companion_computer_id;
    MAV_TYPE vehicle_type;
    MAV_AUTOPILOT autopilot_type;
};