#pragma once

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>

#include <fcntl.h>
#include <sys/time.h>

#include <arpa/inet.h>
#include <common/mavlink.h>
#include "ports/GenericPort.h"

namespace FW
{
    class UDPPort: public GenericPort
    {

    public:

        UDPPort() = default;
        UDPPort(const char *target_ip_, int udp_port_);
        ~UDPPort() override = default;

        bool read_message(mavlink_message_t &message) override;
        bool write_message(const mavlink_message_t &message) override;

        bool is_running() override
        {
            return is_open;
        }
        void start() override;
        void stop() override;

    private:

        mavlink_status_t lastStatus;
        void initialize_defaults();

        const static int BUFF_LEN=2041;
        char buff[BUFF_LEN];
        int buff_ptr;
        int buff_len;
        bool debug;
        const char *target_ip;
        int rx_port;
        int tx_port;
        int sock;
        bool is_open;

        int  _read_port(uint8_t &cp);
        int _write_port(char *buf, unsigned len);

    };

} // FW
