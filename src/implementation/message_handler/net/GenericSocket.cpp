#include "GenericSocket.h"

namespace FW
{

    void GenericSocket::Send(const mavlink_message_t &msg)
    {
        if(is_open)
        {
            out_queue.push_back(msg);
            return;
        }
        std::cout << "[FW-LOG-ERROR]: Tried to send a message when the socket was not open." << std::endl;
    }

    bool GenericSocket::IsOpen() const
    {
        return is_open;
    }

    TSQueue<mavlink_message_t> &GenericSocket::Incoming()
    {
        return incoming_queue;
    }
}
