#pragma once
#include "common/mavlink.h"
#include "infrastructure/TSQueue.h"
#include <iostream>
#include <thread>
#include <atomic>

namespace FW
{
    class GenericSocket : public std::enable_shared_from_this<GenericSocket>
    {
    public:
        GenericSocket() = default;
        virtual bool Start() = 0; //This name convention for UDP is just triggering the reading
        virtual void Close() = 0;

        void Send(const mavlink_message_t& msg);
        bool IsOpen() const;
        TSQueue<mavlink_message_t>& Incoming();
        TSQueue<mavlink_message_t>& IncomingAck();
        virtual ~GenericSocket() = default;
        virtual void WriterThread() = 0;
        virtual void ReaderThread() = 0;
    protected:
        void AddToIncomingQueue(const mavlink_message_t& msg);

        TSQueue<mavlink_message_t> ack_queue;
        TSQueue<mavlink_message_t> out_queue;
        TSQueue<mavlink_message_t> incoming_queue;
        std::atomic<bool> should_reader_thread_run = false;
		std::atomic<bool> should_writer_thread_run = false;
    };
} // FW


