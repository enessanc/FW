#include "FWContext.h"

namespace FW
{

    std::shared_ptr<System> FWContext::DiscoverSystem(const std::string &raw_end_point)
    {
        return _impl.DiscoverSystem(raw_end_point);
    }

} // FW