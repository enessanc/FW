#pragma once
#include "GenericSocket.h"
#include "infrastructure/UDPSocket.h"
#include "utility/StringUtility.h"
#include <optional>

namespace FW
{
    enum InfrastructureType
    {
        Serial = 0,
        UDP,
        TCP
    };

    struct Infrastructure
    {
        InfrastructureType type;
        std::string target;
        int port = -1;
    };

    // Network handler is a facade to socket level communication for library. It detects and creates the socket by given value.
    class NetworkHandler
    {
    public:
        NetworkHandler() = default;
        void SetInfrastructure(const std::string& re);
        void Start();
        void Close();
        void Send(const mavlink_message_t& msg);
        TSQueue<mavlink_message_t>& Incoming();
        TSQueue<mavlink_message_t>& IncomingACK();
        [[nodiscard]] bool IsOpen() const;
        [[nodiscard]] std::string GetEndPoint() const;
    private:
        void ParseRawEndpoint();
        std::optional<Infrastructure> infrastructure;
        std::shared_ptr<GenericSocket> generic_socket = nullptr;
        std::thread reader_thread;
        std::thread writer_thread;
        std::string raw_endpoint;
    };

} // FW

