#pragma once
#include "message_handler/MessageHandler.h"
#include <memory>

namespace FW
{

    class FWImpl
    {
    public:
        FWImpl() = default;
        std::shared_ptr<System> DiscoverSystem(const std::string& raw_endpoint);
        bool Connect(const std::shared_ptr<System>& system);
        void Close();
    private:
        MessageHandler message_handler;
    };
}




