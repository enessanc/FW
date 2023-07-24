#pragma once
#include "System.h"
#include "message_handler/MessageHandler.h"
#include <memory>

namespace FW
{

    typedef std::function<void(std::shared_ptr<System>)> SystemCallbackFunction;

    class FWImpl
    {
    public:
        FWImpl() = default;

        std::shared_ptr<System> DiscoverSystem(const std::string& raw_endpoint);
        void DiscoverSystemAsync(const std::string& raw_endpoint, const SystemCallbackFunction& callback_func);
        MessageHandler* GetMessageHandler();
    private:
        SystemCallbackFunction system_callback_function;
        MessageHandler message_handler;
    };
}




