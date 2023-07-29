#pragma once
#include "message_handler/net/NetworkHandler.h"

namespace FW
{

    class MavlinkCommander
    {
    public:
        MavlinkCommander() = default;
        explicit MavlinkCommander(NetworkHandler* nh) : network_handler(nh) {}
    private:
        NetworkHandler* network_handler;
    };

} // FW

