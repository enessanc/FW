#pragma once
#include <functional>
#include <atomic>
#include <thread>
#include "message_handler/net/NetworkHandler.h"
#include "HashMap.h"

namespace FW
{

    constexpr uint32_t MAXIMUM_SENDER_WINDOW = 128;

    struct CommandMessage
    {
        uint32_t id{};
        mavlink_message_t cmd_msg{};
        bool is_acked = false;
    };


    enum class CommandResultEnum : uint8_t
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

    class CommandResult
    {
        CommandResultEnum result{};
        uint8_t progress{};
        int32_t result_parameter{};
    };

    enum class CommanderState : uint8_t
    {
        WaitForCommand,
        WaitForACK
    };

    enum class CongestionControlState : uint8_t
    {
        SlowStart,
        FastRecovery,
        CongestionAvoidance
    }

    typedef std::function<void(CommandResult)> CommandCallback;

    class MavlinkCommander
    {
    public:
        MavlinkCommander() = default;
        void SetNetworkHandler(NetworkHandler* nh);
        void Start();
        void SendCommand(const CommandMessage& cmd_msg, CommandCallback callback);
        void Finish();
        [[nodiscard]] bool IsRunning() const;
    private:
        void CommandHandler();
        void WaitForCommand();
        void WaitForACK();
    private:
        NetworkHandler* network_handler = nullptr;

        std::thread commander_thread;
        std::atomic<bool> should_thread_run = false;

        TSQueue<CommandMessage> sender_queue;
        CTSL::HashMap<uint32_t, CommandCallback> callback_map;

        std::vector<CommandMessage> sender_window;
        uint32_t sender_window_size = 1;

        TimeoutDetector timeout_detector;
        uint32_t estimated_rtt{};
        uint32_t sample_rtt{};
        uint32_t dev_rtt{};
        uint32_t timeout_duration{};

        CommanderState state = CommanderState::WaitForCommand;
        CongestionControlState congestion_control_state = CongestionControlState::SlowStart;
    };

} // FW

