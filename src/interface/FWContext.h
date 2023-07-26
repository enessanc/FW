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
    private:
        FWImpl _impl;
    };

}

