#include "FWImpl.h"

#include <utility>

namespace FW
{

    std::shared_ptr<System> FWImpl::DiscoverSystem(const std::string& raw_endpoint)
    {
        std::optional<SystemInfo> system_info = message_handler.DiscoverSystemInfo(raw_endpoint);
        if(system_info.has_value())
        {
            auto system = std::make_shared<System>(system_info.value());
            system->impl = this;
        }
        return nullptr;
    }

    void FWImpl::DiscoverSystemAsync(const std::string &raw_endpoint, const SystemCallbackFunction& callback_func)
    {
        system_callback_function = callback_func;
    }

    MessageHandler *FWImpl::GetMessageHandler()
    {
        return &message_handler;
    }

}


