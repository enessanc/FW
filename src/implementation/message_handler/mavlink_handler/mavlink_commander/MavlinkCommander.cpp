#include "MavlinkCommander.h"
#include <utility>

namespace FW
{

    void MavlinkCommander::SetNetworkHandler(NetworkHandler *nh)
    {
        network_handler = nh;
    }

    void MavlinkCommander::SetSystem(std::shared_ptr<System> info)
    {
        system = std::move(info);
    }

    void MavlinkCommander::Start()
    {
        if(!should_thread_run)
        {
            should_thread_run = true;
            CalculateTimeoutDuration(true);
            commander_thread = std::thread([this](){CommandHandler();});
        }
    }

    void MavlinkCommander::SendCommand(const CommandMessage &cmd_msg, const CommandCallback& callback)
    {
        sender_queue.push_back(cmd_msg);
        callback_map.insert(cmd_msg.id, callback);
    }


    bool MavlinkCommander::IsRunning() const
    {
        return should_thread_run;
    }

    void MavlinkCommander::Finish()
    {
        should_thread_run = false;
        if(commander_thread.joinable())
        {
            commander_thread.join();
        }
    }

    void MavlinkCommander::CommandHandler()
    {
        do
        {
            switch(state)
            {
                case CommanderState::WaitForCommand:
                    WaitForCommand();
                    break;
                case CommanderState::WaitForACK:
                    WaitForACK();
                    break;
            }
        } while(should_thread_run);
    }

    void MavlinkCommander::WaitForCommand()
    {
        while(sender_queue.wait_for(100ms) != std::cv_status::timeout)
        {
            while(!sender_queue.empty() && (sender_queue.count() <= sender_window_size))
            {
                sender_window.push_back(sender_queue.front());
            }
            state = CommanderState::WaitForACK;
            break;
        }
    }

    void MavlinkCommander::WaitForACK()
    {
        while (!sender_window.empty())
        {
            SendCommands();

            if (network_handler->IncomingACK().wait_for(timeout_duration) != std::cv_status::timeout)
            {
                finish_sample_rtt = std::chrono::system_clock::now();
                HandleIncomingACKs();
                HandleCongestionControl(false);
            }
            else
            {
                HandleCongestionControl(true);
            }
            CalculateTimeoutDuration(false);
            HandleSenderWindow();
        }
        state = CommanderState::WaitForCommand;
    }


    void MavlinkCommander::SendCommands()
    {
        for (auto& command : sender_window)
        {
            if (&command == &sender_window.at(0))
            {
                start_sample_rtt = std::chrono::system_clock::now();
            }
            switch (command.ack_state)
            {
                case ACKState::NotAckedNotSent:
                {
                    network_handler->Send(EncodeCommand(command));
                    command.ack_state = ACKState::NotAckedButSent;
                    break;
                }
                case ACKState::NotAckedButSent:
                {
                    IncreaseConfirmation(command);
                    network_handler->Send(EncodeCommand(command));
                    break;
                }
                case ACKState::Acked:
                case ACKState::InProgress:
                    break;
                default:
                    std::cout << "[FW]:[LOG-ERROR]: mavlink commander unexpected ack status " << std::endl;
                    break;
            }
        }
    }

    void MavlinkCommander::IncreaseConfirmation(CommandMessage& cmd_msg)
    {
        switch (cmd_msg.type)
        {
            case CommandType::Int:
            {
                // TODO: Investigate if the absence of confirmation field of mavlink_command_int created problems.
                break;
            }
            case CommandType::Long:
            {
                cmd_msg.cmd_long.confirmation++;
                break;
            }
            default:
                std::cout << "[FW]:[LOG-ERROR]: mavlink commander unexpected command type while increasing confirmation" << std::endl;
                break;
        }
    }

    mavlink_message_t MavlinkCommander::EncodeCommand(const CommandMessage& cmd_msg)
    {
        switch (cmd_msg.type)
        {
            case CommandType::Int:
            {
                mavlink_message_t msg{};
                mavlink_msg_command_int_encode(system->system_id, system->component_id, &msg, &cmd_msg.cmd_int);
                return msg;
            }
            case CommandType::Long:
            {
                mavlink_message_t msg{};
                mavlink_msg_command_long_encode(system->system_id, system->component_id, &msg, &cmd_msg.cmd_long);
                return msg;
            }
            default:
            {
                std::cout << "[FW]:[LOG-ERROR]: mavlink commander unexpected command type while encoding command"
                             ", returning invalid msg, that may cause undefined problems" << std::endl;
                mavlink_message_t msg{};
                return msg;
            }
        }
    }

    void MavlinkCommander::HandleIncomingACKs()
    {
        while (!network_handler->IncomingACK().empty())
        {
            mavlink_message_t msg = network_handler->IncomingACK().pop_front();
            mavlink_command_ack_t ack;
            mavlink_msg_command_ack_decode(&msg, &ack);
            for (auto& command: sender_window)
            {
                if (command.id == ack.command)
                {

                        CommandCallback callback;
                        callback_map.find(command.id, callback);
                        CommandResult result = DecodeAck(ack);
                        if (result.result == MAV_RESULT_IN_PROGRESS)
                        {
                            command.ack_state = ACKState::InProgress;
                        }
                        else
                        {
                            command.ack_state = ACKState::Acked;
                        }
                        callback(result);
                }
            }
        }
    }

    CommandResult MavlinkCommander::DecodeAck(const mavlink_command_ack_t& ack)
    {
        CommandResult result;
        result.progress = ack.progress;
        result.result_parameter = ack.result_param2;
        result.result = static_cast<MAV_RESULT>(ack.result);
        return result;
    }

    void MavlinkCommander::HandleCongestionControl(const bool& timeout_happened)
    {
        switch (congestion_control_state)
        {
            case CongestionControlState::SlowStart:
            {
                if(timeout_happened)
                {
                    ssthresh = sender_window_size/2;
                    sender_window_size = 1*MSS;
                }
                else
                {
                    sender_window_size += MSS;
                }

                if(sender_window_size >= ssthresh)
                {
                    congestion_control_state = CongestionControlState::CongestionAvoidance;
                }
                break;
            }
            case CongestionControlState::CongestionAvoidance:
            {
                if(timeout_happened)
                {
                    ssthresh = sender_window_size/2;
                    sender_window_size = 1*MSS;
                    congestion_control_state = CongestionControlState::SlowStart;
                }
                else
                {
                    sender_window_size = sender_window_size + MSS*(MSS/sender_window_size);
                }
                break;
            }
            default:
            {
                std::cout << "[FW]:[LOG-ERROR]: mavlink commander unexpected congestion "
                             "state type while handling congestion control" << std::endl;
                break;
            }
        }


        sender_window_size = std::min(sender_window_size, MAXIMUM_SENDER_WINDOW);
    }

    void MavlinkCommander::HandleSenderWindow()
    {
        while (sender_window.front().ack_state == ACKState::Acked)
        {
            callback_map.erase(sender_window.front().id);
            sender_window.erase(sender_window.begin());
        }

        while ((sender_window.size() <= sender_window_size) && (!sender_queue.empty()))
        {
            sender_window.push_back(sender_queue.front());
        }
    }

    using namespace std::chrono;
    void MavlinkCommander::CalculateTimeoutDuration(const bool& is_first_time)
    {
        std::chrono::nanoseconds estimated_rtt = 0ms;
        std::chrono::nanoseconds sample_rtt = 0ms;
        std::chrono::nanoseconds dev_rtt = 0ms;
        if(is_first_time)
        {
            sample_rtt = system->initial_sample_rtt;
            estimated_rtt = sample_rtt;
            dev_rtt = sample_rtt;
        }
        else
        {
            sample_rtt = duration_cast<nanoseconds>(finish_sample_rtt-start_sample_rtt);
            estimated_rtt = duration_cast<nanoseconds>((1-ALPHA)*estimated_rtt + ALPHA*sample_rtt);
            dev_rtt = duration_cast<nanoseconds>((1-BETA)*dev_rtt + BETA*abs(sample_rtt-estimated_rtt));
        }

        timeout_duration = estimated_rtt + 4*dev_rtt;
    }


} // FW