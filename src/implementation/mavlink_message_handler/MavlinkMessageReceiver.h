#pragma once

#include "net/NetworkHandler.h"
namespace FW
{

    class MavlinkMessageReceiver
    {
    public:
        explicit MavlinkMessageReceiver(NetworkHandler* nh) : network_handler(nh) {}
    private:
        NetworkHandler* network_handler;
    };

} // FW

