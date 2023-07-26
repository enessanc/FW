#include "gtest/gtest.h"
#include "FWContext.h"

using namespace FW;

TEST(FWDiscoverSystem, FWDiscoverSystemNormal)
{
    FWContext context;
    ASSERT_NE(context.DiscoverSystem("udp:127.0.0.1:14540"), nullptr) << "Autopilot did not found.";
}


int main(int argc, char** argv)
{
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}