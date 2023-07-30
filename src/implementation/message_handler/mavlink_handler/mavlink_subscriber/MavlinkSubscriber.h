#pragma once
#include "message_handler/net/NetworkHandler.h"
#include "Subscription.h"

namespace FW
{

    class MavlinkSubscriber
    {
    public:
        MavlinkSubscriber() = default;
    private:
        NetworkHandler* network_handler;
    };

} // FW

