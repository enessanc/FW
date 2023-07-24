#pragma once
#include <memory>
#include "System.h"
#include "FWImpl.h"

namespace FW
{
    class FWContext
    {
    public:
        FWContext() = default;

        std::shared_ptr<System> DiscoverSystem(const std::string& raw_end_point);
        void DiscoverSystemAsync(const std::string& raw_end_point, const SystemCallbackFunction& callback_function);
        bool ConnectSystem();
    private:
        FWImpl _impl;
    };

}

