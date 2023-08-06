#include "MavlinkCommander.h"

namespace FW
{

    void MavlinkCommander::SetNetworkHandler(NetworkHandler *nh)
    {
        network_handler = nh;
    }

    void MavlinkCommander::Start()
    {
        if(!should_thread_run)
        {
            should_thread_run = true;
            commander_thread = std::thread([this](){CommandHandler();});
        }
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

        timeout_detector.Finish();
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
        while(sender_queue.wait_for(100) != std::cv_status::timeout)
        {
            while(!sender_queue.empty() && (sender_queue.count() != MAXIMUM_SENDER_WINDOW))
            {
                sender_window.push_back(sender_queue.front());
            }
            state = CommanderState::WaitForACK;
            break;
        }
    }

    void MavlinkCommander::SendCommand(const CommandMessage &cmd_msg, CommandCallback callback)
    {
        sender_queue.push_back(cmd_msg);
        callback_map.insert(cmd_msg.id,callback);
    }

} // FW