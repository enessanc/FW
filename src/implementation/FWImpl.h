#pragma once
#include "System.h"
#include "message_handler/MessageHandler.h"
#include <memory>

namespace FW
{

    class FWImpl
    {
    public:
        FWImpl() = default;
        std::shared_ptr<System> DiscoverSystem(const std::string& raw_endpoint);
        void Close();
    private:
        MessageHandler message_handler;
    };
}




