#pragma once
#include <common/mavlink.h>

namespace FW
{
    class GenericPort
    {
    public:
        GenericPort()= default;
        virtual ~GenericPort()= default;
        virtual bool read_message(mavlink_message_t &message)=0;
        virtual bool write_message(const mavlink_message_t &message)=0;
        virtual bool is_running()=0;
        virtual void start()=0;
        virtual void stop()=0;
    };

}
