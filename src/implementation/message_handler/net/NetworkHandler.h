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

    class NetworkHandler
    {
    public:
        NetworkHandler() = default;
        void SetInfrastructure(const std::string& raw_endpoint);
        void Start();
        void Close();
        void Send(const mavlink_message_t& msg);
        TSQueue<mavlink_message_t>& Incoming();
        [[nodiscard]] bool IsOpen() const;
    private:
        void ParseRawEndpoint(const std::string& raw_endpoint);
        std::optional<Infrastructure> infrastructure;
        std::shared_ptr<GenericSocket> generic_socket = nullptr;
        std::thread reader_thread;
        std::thread writer_thread;
    };

} // FW

