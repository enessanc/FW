#pragma once
#include "net/NetworkHandler.h"

namespace FW
{

    class MavlinkMessageSender
    {
    public:
        explicit MavlinkMessageSender(NetworkHandler* nh) : network_handler(nh) {}
    private:
        NetworkHandler* network_handler;
    };

} // FW

