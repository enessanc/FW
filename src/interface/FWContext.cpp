#include "FWContext.h"

namespace FW
{

    std::shared_ptr<System> FWContext::DiscoverSystem(const std::string &raw_end_point)
    {
        return _impl.DiscoverSystem(raw_end_point);
    }

    void FWContext::Close()
    {
        _impl.Close();
    }

    bool FWContext::Connect(const std::shared_ptr<System> &system)
    {
        return _impl.Connect(system);
    }

} // FW