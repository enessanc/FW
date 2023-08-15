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
        if(system_future.wait_for(8s) == std::future_status::timeout)
        {
            if (listening_thread.joinable())
            {
                listening_thread.join();
            }
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
        if (network_handler.Incoming().wait_for(6s) != std::cv_status::timeout)
        {
            while (!network_handler.Incoming().empty())
            {
                mavlink_message_t msg = network_handler.Incoming().pop_front();
                if (msg.msgid == MAVLINK_MSG_ID_HEARTBEAT)
                {
                    std::shared_ptr<System> system = std::make_shared<System>();
                    system->system_id = msg.sysid;
                    system->component_id = msg.compid;
                    mavlink_heartbeat_t heartbeat_msg;
                    mavlink_msg_heartbeat_decode(&msg, &heartbeat_msg);
                    system->autopilot_id = static_cast<MAV_AUTOPILOT>(heartbeat_msg.autopilot);
                    system->vehicle_id = static_cast<MAV_TYPE>(heartbeat_msg.type);
                    system->mav_version = heartbeat_msg.mavlink_version;
                    system_info_promise.set_value(system);
                }
            }
            
        }
    }

    //Mavlink connecting is simply requesting specific message (default SYSTEM_TIME) to measure the rtt for
    // command protocol and check if two-way connection can be established.
    bool MessageHandler::Connect(const std::shared_ptr<System>& system)
    {
        const uint16_t CMD_ID = 400;
        mavlink_command_long_t com = { 0 };
        com.target_system    = system->system_id;
        com.target_component = system->component_id;;
        com.command          = CMD_ID;
        com.param1           = 1.0f; // flag >0.5 => start, <0.5 => stop
        com.param2           = 21196;
        mavlink_message_t msg_s;
        mavlink_msg_command_long_encode(system->system_id, MAV_COMP_ID_ALL  ,&msg_s, &com);
        network_handler.Send(msg_s);
        std::cout << "[FW][LOG-DEBUG]: Sent the system time message to measure rtt in connection request. " << std::endl;
        auto start = std::chrono::system_clock::now();
        if(network_handler.IncomingACK().wait_for(10s) != std::cv_status::timeout)
        {
            while(!network_handler.IncomingACK().empty())
            {
                mavlink_message_t msg = network_handler.IncomingACK().pop_front();
                if(msg.msgid == MAVLINK_MSG_ID_COMMAND_ACK)
                {
                    bool result;
                    auto finish = std::chrono::system_clock::now();
                    system->initial_sample_rtt = std::chrono::duration_cast<std::chrono::nanoseconds>(finish - start);
                    mavlink_command_ack_t cmd_ack;
                    mavlink_msg_command_ack_decode(&msg, &cmd_ack);
                    std::cout << "[FW][LOG-INFO]: A command ack was taken. Checking if it is command for system time message request." << std::endl;
                    std::cout << "[FW][LOG-INFO]: Command Number: " << cmd_ack.command << std::endl;
                    std::cout << "[FW][LOG-INFO]: Command Result: " << static_cast<uint32_t>(cmd_ack.result) << std::endl;
                    if(cmd_ack.command == CMD_ID)
                    {
                        std::cout << "[FW][LOG-DEBUG]: The command ack matched the command which was sent. " << std::endl;
                        result = true;
                    }
                    else
                    {
                        std::cout << "[FW][LOG-DEBUG]: The command ack did not match the command which was sent." << std::endl;
                        result = false;
                    }
                    return result;
                }
            }
        }
        return false;
    }



}
