#pragma once
#include "message_handler/net/NetworkHandler.h"
#include <functional>
#include <atomic>
#include <thread>

namespace FW
{

    enum class CommandResult : uint8_t
    {
        Accepted = 0,
        TemporarilyRejected,
        Denied,
        Unsupported,
        Failed,
        InProgress,
        Cancelled,
        CommandLongOnly,
        CommandIntOnly,
        CommandUnsupportedMavFrame
    };

    typedef std::function<void(CommandResult)> CommandCallback;

    class MavlinkCommander
    {
    public:
        MavlinkCommander() = default;
        void SetNetworkHandler(NetworkHandler* nh);
        void Start();
        void Command(const mavlink_message_t& msg);
        void Finish();
        bool IsRunning() const;
    private:
        void CommandHandler();
        void IncreaseSenderWindow();
        void DecreaseSenderWindow();
        void ApplyCallbacks();
    private:
        NetworkHandler* network_handler = nullptr;
        std::thread commander_thread;
        std::atomic<bool> should_thread_run = false;

        TSQueue<mavlink_message_t> sender_queue;
        uint32_t sender_window_size = 1;
    };

} // FW

