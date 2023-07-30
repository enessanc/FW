#pragma once
#include <functional>
#include "SystemInfo.h"
#include "net/NetworkHandler.h"
#include "message_handler/mavlink_handler/mavlink_subscriber/MavlinkSubscriber.h"
#include "message_handler/mavlink_handler/mavlink_commander/MavlinkCommander.h"

namespace FW
{
    class FWImpl;
    typedef std::function<void(std::shared_ptr<SystemInfo>)> SystemInfoCallbackFunction;

    class MessageHandler
    {
    public:
        MessageHandler() = default;
        void Start(const std::string& raw_endpoint);
        std::shared_ptr<SystemInfo> DiscoverSystemInfo();

        void Close();
    private:
        void DiscoverSystemInfoCallback(std::promise<SystemInfo>& system_info_promise);

    private:
        NetworkHandler network_handler;
        MavlinkSubscriber subscriber{};
        MavlinkCommander commander{};
    };

} // FW


