#pragma once
#include "SystemInfo.h"

namespace FW
{

    class FWImpl;

    class System
    {
    public:
        friend class FWImpl;
        System() = default;
        explicit System(const SystemInfo& info) : system_info(info) {}
        [[nodiscard]] SystemInfo GetInfo() const;
    private:
        SystemInfo system_info{};
        FWImpl *impl = nullptr;
    };

} // FW

