#pragma once
#include <utility>
#include "../GenericSocket.h"
#include <mutex>



#ifdef _WIN32
#include <winsock2.h>
#include <WS2tcpip.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif


namespace FW
{
    constexpr std::chrono::milliseconds TRIGGER_WAIT_DURATION = 1000ms;
	constexpr std::chrono::milliseconds READER_THREAD_INTERVAL = 10ms;

    class UDPSocket : public GenericSocket
    {
    public:
        UDPSocket() = default;
        UDPSocket(std::string target_ipv4, const int& port) : target_ipv4(std::move(target_ipv4)), rx_port(port) {}

        bool Start() override;
        void Close() override;
        void WriterThread() override;
        void ReaderThread() override;

    private:
        int _write_port(uint8_t *buffer, uint32_t len);
        int _read_port(uint8_t& cp);
        int close_socket() const;

    private:
        //Related to targets
        std::mutex mut;

        int rx_port{};
        std::string target_ipv4;
        int tx_port = -1;

        // Related to debug
        mavlink_status_t last_status{};
        bool debug = false;

        //Related to socket stuff
        int udp_socket{};
        static const size_t BUFFER_SIZE = sizeof(mavlink_message_t)*10;
        uint8_t receiver_buffer[BUFFER_SIZE]{};
        int buff_ptr{};
        int buff_len{};


    };
}




