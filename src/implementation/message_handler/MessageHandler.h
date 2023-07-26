#pragma once
#include <functional>
#include "SystemInfo.h"
#include "net/NetworkHandler.h"
#include "mavlink_handler/MavlinkSubscriber.h"
#include "mavlink_handler/MavlinkCommander.h"

namespace FW
{
    class FWImpl;
    typedef std::function<void(std::optional<SystemInfo>)> SystemInfoCallbackFunction;

    class MessageHandler
    {
    public:
        MessageHandler();
        std::optional<SystemInfo> DiscoverSystemInfo(const std::string& raw_endpoint);
    private:
        std::optional<Infrastructure> infrastructure{};
        NetworkHandler network_handler;
        MavlinkSubscriber subscriber{};
        MavlinkCommander commander{};
    };

} // FW


