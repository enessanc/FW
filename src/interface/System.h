#pragma once
#include "../implementation/FWImpl.h"
#include "../implementation/SystemImpl.h"


namespace FW
{
    class System
    {
    public:
        explicit System(FWImpl* context);

        void CheckHealth();
        void GetListOfSubsystems();

    private:
        FWImpl* context;
        SystemImpl _impl;
    };

} // FW

