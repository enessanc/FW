#include <future>
#include "MessageHandler.h"

namespace FW
{

    void MessageHandler::Start(const std::string &raw_endpoint)
    {
        network_handler.SetInfrastructure(raw_endpoint);
        network_handler.Start();
    }

    std::shared_ptr<SystemInfo> MessageHandler::DiscoverSystemInfo()
    {
        std::promise<SystemInfo> system_info_promise;
        std::future<SystemInfo> system_info_future = system_info_promise.get_future();
        std::thread listening_thread([this, &system_info_promise](){DiscoverSystemInfoCallback(system_info_promise);});
        if(system_info_future.wait_for(2500ms) == std::future_status::timeout)
        {
            return nullptr;
        }
        if(listening_thread.joinable())
        {
            listening_thread.join();
        }
        return std::make_shared<SystemInfo>(system_info_future.get());
    }

    void MessageHandler::Close()
    {
        if(network_handler.IsOpen())
        {
            network_handler.Close();
        }
    }

    void MessageHandler::DiscoverSystemInfoCallback(std::promise<SystemInfo>& system_info_promise)
    {
        while (network_handler.Incoming().wait_for(2000) != std::cv_status::timeout)
        {
            mavlink_message_t msg = network_handler.Incoming().pop_front();
            if(msg.msgid == MAVLINK_MSG_ID_HEARTBEAT)
            {
                SystemInfo system_info{};
                system_info.system_id = msg.sysid;
                system_info.companion_computer_id = msg.compid;
                mavlink_heartbeat_t heartbeat_msg;
                mavlink_msg_heartbeat_decode(&msg, &heartbeat_msg);
                system_info.autopilot_type = static_cast<MAV_AUTOPILOT>(heartbeat_msg.autopilot);
                system_info.vehicle_type = static_cast<MAV_TYPE>(heartbeat_msg.type);
                system_info_promise.set_value(system_info);
                break;
            }
        }
    }



}
