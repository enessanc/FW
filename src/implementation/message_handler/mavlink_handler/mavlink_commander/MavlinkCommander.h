#pragma once
#include <functional>
#include <atomic>
#include <thread>
#include <algorithm>
#include "message_handler/net/NetworkHandler.h"
#include "HashMap.h"
#include "System.h"

namespace FW
{

    constexpr uint32_t MAXIMUM_SENDER_WINDOW = 128;
    constexpr double ALPHA = 0.125;
    constexpr double BETA = 0.25;
    constexpr uint32_t MSS = 4;
    constexpr uint32_t INITIAL_SSTHRESH = 16;

    enum class CommandType : uint8_t
    {
        Int = 0,
        Long
    };


    enum class ACKState : uint8_t
    {
        NotAckedNotSent = 0,
        NotAckedButSent,
        InProgress,
        Acked
    };

    struct CommandMessage
    {
        uint16_t id{};
        mavlink_command_long_t cmd_long{};
        mavlink_command_int_t cmd_int{};
        CommandType type = CommandType::Long;
        ACKState ack_state = ACKState::NotAckedNotSent;
    };


    struct CommandResult
    {
        MAV_RESULT result{};
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
        CongestionAvoidance
    };

    typedef std::function<void(CommandResult)> CommandCallback;

    class MavlinkCommander
    {

    public:

        MavlinkCommander() = default;
        void SetNetworkHandler(NetworkHandler* nh);
        void SetSystem(std::shared_ptr<System> info);
        void Start();
        void SendCommand(const CommandMessage& cmd_msg, const CommandCallback& callback);
        void Finish();
        [[nodiscard]] bool IsRunning() const;

    private:

        void CommandHandler();
        void WaitForCommand();
        void WaitForACK();
        void SendCommands();
        static void IncreaseConfirmation(CommandMessage& cmd_msg);
        mavlink_message_t EncodeCommand(const CommandMessage& cmd_msg);
        void HandleIncomingACKs();
        static CommandResult DecodeAck(const mavlink_command_ack_t& ack);
        void HandleCongestionControl(const bool& timeout_happened);
        void HandleSenderWindow();
        void CalculateTimeoutDuration(const bool& is_first_time);

    private:

        NetworkHandler* network_handler = nullptr;
        std::shared_ptr<System> system = nullptr;

        std::thread commander_thread;
        std::atomic<bool> should_thread_run = false;

        TSQueue<CommandMessage> sender_queue;
        CTSL::HashMap<uint16_t, CommandCallback> callback_map;

        std::vector<CommandMessage> sender_window;


        std::chrono::time_point<std::chrono::system_clock> start_sample_rtt;
        std::chrono::time_point<std::chrono::system_clock> finish_sample_rtt;

        std::chrono::nanoseconds timeout_duration = 0ns;

        CommanderState state = CommanderState::WaitForCommand;

        CongestionControlState congestion_control_state = CongestionControlState::SlowStart;
        uint32_t sender_window_size = 1*MSS;
        uint32_t ssthresh = INITIAL_SSTHRESH;

    };

} // FW


