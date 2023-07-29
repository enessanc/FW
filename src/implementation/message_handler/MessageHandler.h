#pragma once
#include <functional>
#include "SystemInfo.h"
#include "net/NetworkHandler.h"
#include "message_handler/mavlink_handler/mavlink_subscriber/MavlinkSubscriber.h"
#include "message_handler/mavlink_handler/mavlink_commander/MavlinkCommander.h"

namespace FW
{
    class FWImpl;
    typedef std::function<void(std::optional<SystemInfo>)> SystemInfoCallbackFunction;

    class MessageHandler
    {
    public:
        MessageHandler();
        std::optional<SystemInfo> DiscoverSystemInfo(const std::string& raw_endpoint);
        void Close();
    private:
        std::optional<Infrastructure> infrastructure{};
        NetworkHandler network_handler;
        MavlinkSubscriber subscriber{};
        MavlinkCommander commander{};
    };

} // FW


