#pragma once
#include <algorithm>
#include <utility>
#include "../Connection.h"
#include "common/mavlink.h"
#include "TSQueue.h"
#include "asio.hpp"

namespace FW
{
class UDPConnection : public Connection
    {
    public:
        UDPConnection(asio::io_context& context, asio::ip::udp::socket& socket,
                      TSQueue<mavlink_message_t>& q_in, asio::ip::udp::resolver::results_type end_point)
        : context(context) , socket(std::move(socket)), messages_in(q_in), end_point(std::move(end_point)) {}

        void Connect() override;
        void Disconnect() override;
        bool IsConnected() override;
        void Send(const mavlink_message_t& msg) override;
    protected:
        void WriteMessage() override;
        void AddIncomingQueue() override;
        void ReadMessage() override;
        asio::ip::udp::socket socket;
        asio::io_context& context;
        asio::ip::udp::resolver::results_type end_point;
        TSQueue<mavlink_message_t> messages_out;
        TSQueue<mavlink_message_t>& messages_in;
        mavlink_message_t temp_msg_in{};
    };
}




