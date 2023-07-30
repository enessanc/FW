#include "FWImpl.h"

#include <utility>

namespace FW
{

    std::shared_ptr<System> FWImpl::DiscoverSystem(const std::string& raw_endpoint)
    {
        message_handler.Start(raw_endpoint);
        std::shared_ptr<SystemInfo> system_info = message_handler.DiscoverSystemInfo();
        if(system_info)
        {
            auto system = std::make_shared<System>(*system_info);
            system->impl = this;
            return system;
        }
        return nullptr;
    }

    void FWImpl::Close()
    {
        message_handler.Close();
    }

}


