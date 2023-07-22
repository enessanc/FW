#pragma once
#include "asio.hpp"
#include "common/mavlink.h"
#include <string>

namespace FW
{
class Connection : public std::enable_shared_from_this<Connection>
    {
    public:
        Connection() = default;

        virtual bool Discover(const std::string& s) = 0;
        virtual void Disconnect() = 0;
        virtual bool IsConnected() = 0;
        virtual void Send(const mavlink_message_t& msg) = 0;
    protected:
        virtual void ReadMessage() = 0;
        virtual void WriteMessage() = 0;
        virtual void AddIncomingQueue() = 0;
        bool is_connected = false;

    };
} // FW


