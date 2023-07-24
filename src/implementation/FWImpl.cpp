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
        message_handler.DiscoverSystemInfoAsync(raw_endpoint,
                        [this](std::optional<SystemInfo> system_info)
                        {
                            if(system_info.has_value())
                            {
                                std::shared_ptr<System> system = std::make_shared<System>();
                                system->impl = this;
                                this->system_callback_function(system);
                            }
                            else
                            {
                                this->system_callback_function(nullptr);
                            }
                        }
                        );
    }

    MessageHandler *FWImpl::GetMessageHandler()
    {
        return &message_handler;
    }

}


