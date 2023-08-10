#pragma once
#include <functional>
#include "net/NetworkHandler.h"
#include "message_handler/mavlink_handler/mavlink_subscriber/MavlinkSubscriber.h"
#include "message_handler/mavlink_handler/mavlink_commander/MavlinkCommander.h"

namespace FW
{
    // The handler of messaging logic. It encapsulates the mavlink heartbeat-ping-commander-subscriber(FW defined) protocols. 
    // It governs all aspects of communications' so it is a singleton..
    class MessageHandler
    {
    public:
        MessageHandler() = default;
        void StartNetworkHandler(const std::string& raw_endpoint);
        void StartCommander(std::shared_ptr<System> system);
        void StartSubscriber(std::shared_ptr<System> system);

        std::shared_ptr<System> DiscoverSystem();
        bool Connect(const std::shared_ptr<System>& system);

        void SendCommand(const CommandMessage& cmd_msg, const CommandCallback& callback);
        void SubscribeMessage();

        void Close();
    private:
        void DiscoverSystemInfoCallback(std::promise<std::shared_ptr<System>>& system_info_promise);
    private:

        NetworkHandler network_handler;
        MavlinkSubscriber subscriber{};
        MavlinkCommander commander{};
    };

} // FW


