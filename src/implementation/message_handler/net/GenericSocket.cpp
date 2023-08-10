#include "GenericSocket.h"

namespace FW
{

    void GenericSocket::Send(const mavlink_message_t &msg)
    {
        if(IsOpen())
        {
            out_queue.push_back(msg);
            return;
        }
        std::cout << "[FW-LOG-ERROR]: Tried to send a message when the socket was not open." << std::endl;
    }

    bool GenericSocket::IsOpen() const
    {
		return (should_reader_thread_run && should_writer_thread_run);
    }

    TSQueue<mavlink_message_t> &GenericSocket::Incoming()
    {
        return incoming_queue;
    }

    TSQueue<mavlink_message_t> &GenericSocket::IncomingAck()
    {
        return ack_queue;
    }

    void GenericSocket::AddToIncomingQueue(const mavlink_message_t &msg)
    {
        if(msg.msgid == MAVLINK_MSG_ID_COMMAND_ACK)
        {
            ack_queue.push_back(msg);
        }
        else
        {
            //TODO : The coming mssages should be filtered by a subscription protocol.
            if(msg.msgid == MAVLINK_MSG_ID_SYSTEM_TIME || msg.msgid == MAVLINK_MSG_ID_HEARTBEAT)
            {
                incoming_queue.push_back(msg);
            }
        }
    }

}
