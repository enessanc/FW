#include "FWContext.h"

namespace FW
{
    std::shared_ptr<System> FWContext::FindSystem()
    {
        return _impl.FindSystem();
    }

    bool FWContext::ConnectSystem()
    {
        return _impl.ConnectSystem();
    }


} // FW