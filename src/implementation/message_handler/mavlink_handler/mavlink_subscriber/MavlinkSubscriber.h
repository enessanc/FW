#pragma once
#include "message_handler/net/NetworkHandler.h"

namespace FW
{

    class MavlinkSubscriber
    {
    public:
        MavlinkSubscriber() = default;
        explicit MavlinkSubscriber(NetworkHandler* nh) : network_handler(nh) {}
    private:
        NetworkHandler* network_handler;
    };

} // FW

