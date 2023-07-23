#pragma once
#include "System.h"
#include "mavlink_message_handler/net/NetworkHandler.h"
#include <memory>

namespace FW
{
    class FWImpl
    {
    public:
        FWImpl() = default;
        std::shared_ptr<System> FindSystem();
        bool ConnectSystem();
    };
}




