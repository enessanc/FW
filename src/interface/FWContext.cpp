#include "FWContext.h"

namespace FW
{

    std::shared_ptr<System> FWContext::DiscoverSystem(const std::string &raw_end_point)
    {
        return _impl.DiscoverSystem(raw_end_point);
    }

    void FWContext::DiscoverSystemAsync(const std::string &raw_end_point, const SystemCallbackFunction& callback_function)
    {
        _impl.DiscoverSystemAsync(raw_end_point, callback_function);
    }


} // FW