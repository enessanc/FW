#pragma once
#include <algorithm>
#include "../Connection.h"
#include "common/mavlink.h"
#include "TSQueue.h"
#include "asio.hpp"

namespace FW
{
class UDPConnection : public Connection
    {
    public:
        UDPConnection(asio::io_context& context, asio::ip::udp::socket socket, TSQueue<mavlink_message_t>& q_in)
        : asio_context(context) , socket(std::move(socket)), messages_in(q_in) {}


        bool Discover(const std::string& s) override;
        void Disconnect() override;
        bool IsConnected() override;
        void Send(const mavlink_message_t& msg) override;

    protected:
        void ReadMessage() override;
        void WriteMessage() override;
        void AddIncomingQueue() override;

    private:
        void ResolveEndPoint(const std::string& end_point);
    private:
        asio::ip::udp::socket socket;
        asio::io_context& asio_context;
        asio::ip::udp::endpoint end_point;
        TSQueue<mavlink_message_t> messages_out;
        TSQueue<mavlink_message_t>& messages_in;
        mavlink_message_t temp_msg_in{};

    };
}




