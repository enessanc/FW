#include "gtest/gtest.h"
#include "FWContext.h"

using namespace FW;

constexpr int MAX_DURATION_FOR_TIMEOUT = 1100;

TEST(FWMessageHandler, FWTestTimeoutDetector)
{
    int timeout_duration = 500;
    TimeoutDetector timeout_detector;
    std::promise<void> timeout_promise;
    std::future<void> timeout_future = timeout_promise.get_future();

    timeout_detector.SetTimeoutDuration(timeout_duration);
    timeout_detector.SetTimeoutPromise(&timeout_promise);

    std::atomic<bool> should_thread_run = true;
    std::thread timeout_actor([&timeout_future,&timeout_detector,&timeout_duration,&should_thread_run,&timeout_promise]
    {
        do
        {
            if(timeout_future.wait_for(100ms) != std::future_status::timeout)
            {
                if(timeout_duration*2 <= MAX_DURATION_FOR_TIMEOUT)
                {
                    std::cout << "[FWTestTimeoutDetector_TimeoutActorThread]: The timeout detector detected timeout, but currently "
                                 "timeout duration is longer than max duration so the duration will be increased." << std::endl;
                    timeout_duration *= 2;
                    timeout_detector.SetTimeoutDuration(timeout_duration);
                    timeout_promise = std::promise<void>();
                    timeout_future = timeout_promise.get_future();
                    timeout_detector.Reset();
                    timeout_detector.SetTimeoutPromise(&timeout_promise);
                }
                else
                {
                    std::cout << "[FWTestTimeoutDetector_TimeoutActorThread]: The timeout detector detected timeout and currently "
                                 "timeout duration is shorter than max duration so the timeout actor will finish everything." << std::endl;
                    timeout_promise = std::promise<void>();
                    timeout_future = timeout_promise.get_future();
                    timeout_detector.Reset();
                    timeout_detector.SetTimeoutPromise(&timeout_promise);
                    timeout_detector.Finish();
                    should_thread_run= false;
                }
            }
        } while(should_thread_run);
    });

    timeout_detector.Start();
    std::this_thread::sleep_for(200ms);
    timeout_detector.Reset();
    std::cout << "[FWTestTimeoutDetector_MainThread]: The reset of timeout detector is successful after 1000 ms." << std::endl;


    if(timeout_actor.joinable())
    {
        timeout_actor.join();
        std::cout << "[FWTestTimeoutDetector_MainThread]: The timeout detector test successfully executed." << std::endl;
    }
}

TEST(FWMessageHandler, FWDiscoverSystemOnce)
{
    FWContext context;
    std::shared_ptr<System> system = context.DiscoverSystem("udp:127.0.0.1:14540");
    ASSERT_NE(system,nullptr) << "Autopilot did not found, please check its existence.";

    std::cout << "Autopilot found with following specs: " << std::endl;
    std::cout << "[System ID]:" << system->GetInfo().system_id << std::endl;
    std::cout << "[Autopilot Type]:" << system->GetInfo().autopilot_type<< std::endl;
    std::cout << "[Companion Computer ID]:" << system->GetInfo().companion_computer_id << std::endl;
    std::cout << "[Vehicle Type]:" << system->GetInfo().vehicle_type << std::endl;
    context.Close();
}


int main(int argc, char** argv)
{
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}