#include "MavlinkCommander.h"

namespace FW
{

    void MavlinkCommander::SetNetworkHandler(NetworkHandler *nh)
    {
        network_handler = nh;
    }
} // FW