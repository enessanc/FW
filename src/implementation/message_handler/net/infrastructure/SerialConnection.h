#pragma once
#include "../Connection.h"
#include "common/mavlink.h"
#include "TSQueue.h"
#include "asio.hpp"

namespace FW
{
    class SerialConnection : public Connection
    {

    private:
        asio::serial_port serial_port;
        asio::io_context& asio_context;
        TSQueue<mavlink_message_t> messages_out;
        TSQueue<mavlink_message_t>& messages_in;
        mavlink_message_t temp_msg_in;
    };

} // FW


