#pragma once
#include <optional>
#include <iostream>

#include "SystemInfo.h"
#include "net/NetworkHandler.h"
#include "mavlink_handler/MavlinkSubscriber.h"
#include "mavlink_handler/MavlinkCommander.h"
#include "utility/StringUtility.h"

namespace FW
{
    class FWImpl;
    typedef std::function<void(std::optional<SystemInfo>)> SystemInfoCallbackFunction;

    class MessageHandler
    {
    public:
        MessageHandler();
        std::optional<SystemInfo> DiscoverSystemInfo(const std::string& raw_endpoint);
        void DiscoverSystemInfoAsync(const std::string& raw_endpoint, SystemInfoCallbackFunction callback_function);
    private:
        void ParseEndpoint(const std::string&);
        std::optional<Infrastructure> infrastructure{};
        NetworkHandler network_handler;
        MavlinkSubscriber subscriber{};
        MavlinkCommander commander{};
    };

} // FW


