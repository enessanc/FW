#include "FWImpl.h"

#include <utility>

namespace FW
{

    std::shared_ptr<System> FWImpl::DiscoverSystem(const std::string& raw_endpoint)
    {
        //We should first start message handler to discover systems.
        message_handler.StartNetworkHandler(raw_endpoint);
        return message_handler.DiscoverSystem();
    }

    bool FWImpl::Connect(const std::shared_ptr<System>& system)
    {
        return message_handler.Connect(system);
    }

    void FWImpl::Close()
    {
        message_handler.Close();
    }

}


