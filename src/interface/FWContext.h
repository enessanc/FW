#pragma once
#include "FWImpl.h"

namespace FW
{
    // The main handler of FW library uses bridge pattern to encapsulate implementation of FW.
    class FWContext
    {
    public:
        FWContext() = default;
        std::shared_ptr<System> DiscoverSystem(const std::string& raw_end_point);
        bool Connect(const std::shared_ptr<System>& system);
        void Close();
    private:
        FWImpl _impl;
    };

}

