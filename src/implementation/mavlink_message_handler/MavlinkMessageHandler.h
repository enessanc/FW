#pragma once
#include "net/NetworkHandler.h"
#include "MavlinkMessageReceiver.h"
#include "MavlinkMessageSender.h"

namespace FW
{

    class MavlinkMessageHandler
    {
        MavlinkMessageHandler() : receiver(&network_handler), sender(&network_handler) {}
    private:
        NetworkHandler network_handler;
        MavlinkMessageReceiver receiver;
        MavlinkMessageSender sender;
    };

} // FW


