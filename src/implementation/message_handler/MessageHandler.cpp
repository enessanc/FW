#include <future>
#include <utility>
#include "MessageHandler.h"

namespace FW
{

    void MessageHandler::StartNetworkHandler(const std::string &raw_endpoint)
    {
        network_handler.SetInfrastructure(raw_endpoint);
        network_handler.Start();
    }

    void MessageHandler::StartCommander(std::shared_ptr<System> system)
    {
        commander.SetNetworkHandler(&network_handler);
        commander.SetSystem(std::move(system));

    }

    void MessageHandler::SendCommand(const CommandMessage &cmd_msg, const CommandCallback &callback)
    {
        commander.SendCommand(cmd_msg,callback);
    }

    void MessageHandler::StartSubscriber(std::shared_ptr<System> system)
    {

    }

    void MessageHandler::SubscribeMessage()
    {

    }

    void MessageHandler::Close()
    {
        if(network_handler.IsOpen())
        {
            network_handler.Close();
        }
        if(commander.IsRunning())
        {
            commander.Finish();
        }
    }

    std::shared_ptr<System> MessageHandler::DiscoverSystem()
    {
        std::promise<std::shared_ptr<System>> system_promise;
        std::future<std::shared_ptr<System>> system_future = system_promise.get_future();
        std::thread listening_thread([this, &system_promise](){DiscoverSystemInfoCallback(system_promise);});
        if(system_future.wait_for(2500ms) == std::future_status::timeout)
        {
            return nullptr;
        }
        if(listening_thread.joinable())
        {
            listening_thread.join();
        }
        std::shared_ptr<System> system = system_future.get();
        return system;
    }

    void MessageHandler::DiscoverSystemInfoCallback(std::promise<std::shared_ptr<System>>& system_info_promise)
    {
        while (network_handler.Incoming().wait_for(2000ms) != std::cv_status::timeout)
        {
            mavlink_message_t msg = network_handler.Incoming().pop_front();
            if(msg.msgid == MAVLINK_MSG_ID_HEARTBEAT)
            {
                std::shared_ptr<System> system = std::make_shared<System>();
                system->system_id = msg.sysid;
                system->component_id = msg.compid;
                mavlink_heartbeat_t heartbeat_msg;
                mavlink_msg_heartbeat_decode(&msg, &heartbeat_msg);
                system->autopilot_id = static_cast<MAV_AUTOPILOT>(heartbeat_msg.autopilot);
                system->vehicle_id = static_cast<MAV_TYPE>(heartbeat_msg.type);
                system_info_promise.set_value(system);
                break;
            }
        }
    }

    bool MessageHandler::Connect(const std::shared_ptr<System>& system)
    {
        mavlink_command_long_t msg_request_command;

        msg_request_command.command = 512;
        msg_request_command.param1 = MAVLINK_MSG_ID_SYSTEM_TIME;
        mavlink_message_t msg_s;
        mavlink_msg_command_long_encode(system->system_id, system->component_id,&msg_s, &msg_request_command);
        network_handler.Send(msg_s);
        auto start = std::chrono::system_clock::now();
        if(network_handler.Incoming().wait_for(4000ms) != std::cv_status::timeout)
        {
            if(!network_handler.Incoming().empty())
            {
                mavlink_message_t msg = network_handler.Incoming().front();
                if(msg.msgid == MAVLINK_MSG_ID_SYSTEM_TIME)
                {
                    auto finish = std::chrono::system_clock::now();
                    system->initial_sample_rtt = std::chrono::duration_cast<std::chrono::nanoseconds>(finish - start);
                    mavlink_system_time_t system_time;
                    mavlink_msg_system_time_decode(&msg, &system_time);
                    std::cout << "[FW][LOG-DEBUG]: System is connected. " << std::endl;
                    std::cout << "[FW][LOG-INFO]: System Timestamp (UNIX epoch Time)(us): "
                              << system_time.time_unix_usec << std::endl;
                    std::cout << "[FW][LOG-INFO]: System Timestamp (Time Since Boot)(ms): "
                              << system_time.time_boot_ms << std::endl;
                    if (!network_handler.IncomingACK().empty())
                    {
                        std::cout << "[FW][LOG-TRACE]: The command ack messages are deleted to ensure clarity. "
                                  << std::endl;
                        network_handler.IncomingACK().clear();
                    }
                    return true;
                }
            }
        }
        if(!network_handler.IncomingACK().empty())
        {
            std::cout << "[FW][LOG-TRACE]: The command ack messages are deleted to ensure clarity. " << std::endl;
            network_handler.IncomingACK().clear();
        }
        return false;
    }



}
