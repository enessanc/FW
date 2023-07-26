#include <future>
#include "MessageHandler.h"

namespace FW
{

    MessageHandler::MessageHandler()
    {
        commander = MavlinkCommander(&network_handler);
        subscriber = MavlinkSubscriber(&network_handler);
    }

    std::optional<SystemInfo> MessageHandler::DiscoverSystemInfo(const std::string &raw_endpoint)
    {

        network_handler.SetInfrastructure(raw_endpoint);
        network_handler.Start();

        std::promise<std::optional<SystemInfo>> system_info_promise;
        std::future<std::optional<SystemInfo>> system_info_future = system_info_promise.get_future();

        auto listen_heartbeats_async = [this, &system_info_promise]()
                        {
                            while (network_handler.Incoming().wait_for(2000) != std::cv_status::timeout)
                            {
                                mavlink_message_t msg = network_handler.Incoming().pop_front();
                                if(msg.msgid == MAVLINK_MSG_ID_HEARTBEAT)
                                {
                                    std::optional<SystemInfo> system_info;

                                    system_info->system_id = msg.sysid;
                                    system_info->companion_computer_id = msg.compid;

                                    mavlink_heartbeat_t heartbeat_msg;
                                    mavlink_msg_heartbeat_decode(&msg, &heartbeat_msg);
                                    system_info->autopilot_type = static_cast<MAV_AUTOPILOT>(heartbeat_msg.autopilot);
                                    system_info->vehicle_type = static_cast<MAV_TYPE>(heartbeat_msg.type);
                                    system_info_promise.set_value(system_info);
                                }
                            }
                        };

        std::thread listening_thread = std::thread(listen_heartbeats_async);
        if(system_info_future.wait_for(1000000ms) == std::future_status::timeout)
        {
            std::optional<SystemInfo> system_info_zero;
            return system_info_zero;
        }
        return system_info_future.get();
    }
}
