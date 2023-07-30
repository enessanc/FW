#include "TimeoutDetector.h"

namespace FW
{
    void TimeoutDetector::SetTimeoutPromise(std::promise<void> *promise)
    {
        timeout_promise = promise;
    }

    void TimeoutDetector::SetTimeoutDuration(const uint32_t &milliseconds)
    {
        max_duration = milliseconds;
    }

    void TimeoutDetector::Start()
    {
        reset_future = reset_promise.get_future();
        should_thread_run = true;
        timeout_detector_thread = std::thread([this](){HandleTimeoutDetector();});
    }

    void TimeoutDetector::Reset(const TimeoutDetector::RefreshThread &type)
    {
        reset_promise.set_value();
        RefreshFutureAndPromises(type);
    }

    void TimeoutDetector::Finish()
    {
        should_thread_run = false;
        if(timeout_detector_thread.joinable())
        {
            timeout_detector_thread.join();
        }
    }

    void TimeoutDetector::RefreshFutureAndPromises(const TimeoutDetector::RefreshThread &type)
    {
            std::scoped_lock lock(mut);
            if(type == RefreshThread::Out)
            {
                reset_promise = std::promise<void>();
            }
            else
            {
                reset_future = reset_promise.get_future();
            }
    }

    void TimeoutDetector::HandleTimeoutDetector()
    {
        do
        {
            if(reset_future.wait_for(max_duration*1ms) == std::future_status::timeout)
            {
                timeout_promise->set_value();
            }
            else
            {
                RefreshFutureAndPromises(RefreshThread::In);
            }
        } while (should_thread_run);
    }
} // FW