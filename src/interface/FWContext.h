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

        std::shared_ptr<System> FindSystem();
        bool ConnectSystem();

    private:
        FWImpl _impl;
    };

}

