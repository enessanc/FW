#include "FWContext.h"

bool test_connect()
{
    FW::FWContext context;
    std::shared_ptr<FW::System> system = context.DiscoverSystem("udp:127.0.0.1:14540");
    if (system == nullptr)
    {
        std::cout << "Autopilot did not found, please check its existence." << std::endl;
        context.Close();
        return false;
    }

    std::cout << "Autopilot found with following specs: " << std::endl;
    std::cout << "[SystemImpl ID]:" << system->system_id << std::endl;
    std::cout << "[Autopilot Type]:" << system->autopilot_id << std::endl;
    std::cout << "[Companion Computer ID]:" << system->component_id << std::endl;
    std::cout << "[Vehicle Type]:" << system->vehicle_id << std::endl;

    if(!context.Connect(system))
    {
        std::cout << "The connection to system was unsuccessful.\n";
        context.Close();
        return false;
    }

    std::cout << "The connection to system was succesfull." << std::endl;
    context.Close();
    return true;
}


int main()
{
    if (!test_connect())
    {
        return -1;
    }

	return 0;
}