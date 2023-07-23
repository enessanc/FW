#pragma once
#include "asio.hpp"
#include "Connection.h"
#include "infrastructure/UDPConnection.h"
#include "infrastructure/SerialConnection.h"
#include "infrastructure/TSQueue.h"
#include "utility/StringUtility.h"

namespace FW
{
    enum InfrastructureType
    {
        Serial = 0,
        UDP,
        TCP
    };


    class NetworkHandler
    {
    public:
        NetworkHandler() : udp_socket(context), tcp_socket(context), serial_port(context) {};

        bool Discover(const InfrastructureType& type, const std::string& host, const int& port = -1);
        bool Connect();

        void Disconnect();
        bool IsConnected();

        void EnableAsyncIO();

        void Send(const mavlink_message_t& msg);
        TSQueue<mavlink_message_t>& Incoming();
    private:
        asio::io_context context;
        std::thread thread_for_context;
        bool is_async_io_enabled = false;

        asio::ip::udp::socket udp_socket;
        asio::ip::tcp::socket tcp_socket;
        asio::serial_port serial_port;

        std::unique_ptr<Connection> connection = nullptr;
        TSQueue<mavlink_message_t> in_messages;
    };

} // FW

