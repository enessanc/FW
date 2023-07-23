#pragma once
#include "asio.hpp"
#include "common/mavlink.h"

namespace FW
{
class Connection : public std::enable_shared_from_this<Connection>
    {
    public:
        Connection() = default;
        virtual bool Discover() = 0;
        virtual bool Connect() = 0;
        virtual void Disconnect() = 0;
        virtual bool IsConnected() = 0;
        virtual void Send(const mavlink_message_t& msg) = 0;
        virtual ~Connection() = default;
    protected:
        virtual void ReadMessage() = 0;
        virtual void WriteMessage(const mavlink_message_t& msg) = 0;
        virtual void AddIncomingQueue(const mavlink_message_t& msg)= 0;
        bool is_connected = false;
    };
} // FW


