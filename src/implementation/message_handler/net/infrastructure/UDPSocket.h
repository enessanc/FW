#pragma once
#include <utility>
#include "../GenericSocket.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

namespace FW
{
    static const int TRIGGER_WAIT_DURATION = 10000;
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

    private:
        //Related to targets
        int rx_port{};
        std::string target_ipv4;
        int tx_port = -1;

        // Related to debug
        mavlink_status_t last_status{};
        bool debug = true;

        //Related to socket stuff
        int udp_socket{};
        static const size_t BUFFER_SIZE = sizeof(mavlink_message_t)*10;
        uint8_t receiver_buffer[BUFFER_SIZE]{};
        int buff_ptr{};
        int buff_len{};


    };
}




