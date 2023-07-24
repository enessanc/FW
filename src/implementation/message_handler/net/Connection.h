#pragma once
#include "asio.hpp"
#include "common/mavlink.h"
#include <iostream>

namespace FW
{

    typedef void (*callback_function)();

    class Connection : public std::enable_shared_from_this<Connection>
    {
    public:
        Connection() = default;
        virtual void Connect() = 0; //This name convention for UDP is just triggering the reading
        virtual void Disconnect() = 0;
        virtual bool IsConnected() = 0;
        virtual void Send(const mavlink_message_t& msg) = 0;
        virtual ~Connection() = default;
    protected:
        virtual void ReadMessage() = 0;
        virtual void WriteMessage() = 0;
        virtual void AddIncomingQueue()= 0;
    };
} // FW


