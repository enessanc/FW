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
        std::optional<SystemInfo> system_info_zero;
        ParseEndpoint(raw_endpoint);
        if(!infrastructure.has_value())
        {
            return system_info_zero;
        }
        network_handler.StartAsyncIO(infrastructure.value());


        auto listen_heartbeats_async = [this]()
                        {
                            network_handler.Incoming().wait();
                            size_t message_count = 0;
                            while (message_count < 10 && !network_handler.Incoming().empty())
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
                                    return system_info;
                                }
                                message_count++;
                            }
                        };
        std::future<std::optional<SystemInfo>> system_info_future = std::async(std::launch::async,listen_heartbeats_async);
        if (system_info_future.wait_for(std::chrono::seconds(4)) == std::future_status::timeout)
        {
            std::cerr << "[FW-LOG-DEBUG]: No autopilot found." << std::endl;
            return system_info_zero;
        }
        return system_info_future.get();
    }

    void MessageHandler::ParseEndpoint(const std::string& raw_end_point)
    {
        infrastructure.reset();
        std::vector<std::string> strings = StringUtility::custom_split(raw_end_point, ':');
        if(strings.size() == 3 || strings.size() == 2)
        {
            if(strings.at(0) == "serial" || strings.at(0) == "udp" || strings.at(0) == "tcp")
            {
                if(strings.at(0) == "serial")
                {
                    infrastructure->type = InfrastructureType::Serial;
                    infrastructure->target = strings.at(1);
                    infrastructure->port = -1;
                    return;
                }
                if(strings.at(0) == "udp")
                {
                    infrastructure->type = InfrastructureType::UDP;
                    infrastructure->target = strings.at(1);
                    infrastructure->port = std::stoi(strings.at(2));
                    return;
                }
                if(strings.at(0) == "tcp")
                {
                    infrastructure->type = InfrastructureType::TCP;
                    infrastructure->target = strings.at(1);
                    infrastructure->port = std::stoi(strings.at(2));
                    return;
                }
            }
        }
        std::cout << "[FW-LOG-ERROR] : The provided end point string could not be parsed. " << std::endl;
        infrastructure.reset();
    }

    void MessageHandler::DiscoverSystemInfoAsync(const std::string &raw_endpoint,
                                                 SystemInfoCallbackFunction callback_function)
    {
        ParseEndpoint(raw_endpoint);
        if(infrastructure.has_value())
        {
            network_handler.StartAsyncIO(infrastructure.value());
        }
        else
        {
            return;
        }

        //TODO: Desing a work queue to run a seperate thread in message handler for two simple case to actually succed this asynchronously
    }
}
