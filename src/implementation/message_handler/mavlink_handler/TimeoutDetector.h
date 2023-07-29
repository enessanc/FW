#pragma once
#include <chrono>
#include <future>
#include <thread>
#include <atomic>
#include <mutex>

namespace FW
{
    using namespace std::literals;

    class TimeoutDetector
    {
        enum class RefreshThread
        {
            Out,
            In
        };
    public:
        TimeoutDetector() = default;
        void SetTimeoutPromise(std::promise<void>* promise);
        void SetTimeoutDuration(const uint32_t& milliseconds);
        void Start();
        void Reset(const RefreshThread& type = RefreshThread::Out);
        void Finish();
    private:
        void RefreshFutureAndPromises(const RefreshThread& type = RefreshThread::Out);
        void HandleTimeoutDetector();

        std::thread timeout_detector_thread;
        std::promise<void>* timeout_promise = nullptr;
        std::promise<void> reset_promise;
        std::future<void> reset_future;
        std::mutex mut;
        std::atomic<bool> should_thread_run = false;
        uint32_t max_duration{};
    };

} // FW
