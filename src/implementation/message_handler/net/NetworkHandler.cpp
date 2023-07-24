#include "NetworkHandler.h"
#include <iostream>

namespace FW
{

    void NetworkHandler::Disconnect()
    {
        if(connection->IsConnected())
        {
            connection->Disconnect();
        }
        else
        {
            std::cout << "[FW-LOG-ERROR]: To disconnect, you first need to connect to system." << std::endl;
        }

        context.stop();
        if (thread_for_context.joinable())
        {
            thread_for_context.join();
        }
        Connection* pointer_to_delete = connection.release();
        delete pointer_to_delete;
        is_context_running  = false;
    }

    bool NetworkHandler::IsConnected()
    {
        if(connection)
        {
            return connection->IsConnected();
        }
        return false;
    }

    void NetworkHandler::Send(const mavlink_message_t &msg)
    {
        if(connection)
        {
            connection->Send(msg);
        }
        else
        {
            std::cout << "[FW-LOG-ERROR]: To send a message, you first need to discover a system." << std::endl;
        }
    }

    TSQueue<mavlink_message_t> &NetworkHandler::Incoming()
    {
        return in_messages;
    }

    void NetworkHandler::StartAsyncIO(const Infrastructure &infrastructure)
    {
        if(!is_context_running)
        {
            InitConnection(infrastructure);
            connection->Connect();
            thread_for_context = std::thread([this](){context.run();});
            is_context_running = true;
        }
    }

    void NetworkHandler::InitConnection(const Infrastructure &infrastructure)
    {
        switch(infrastructure.type)
        {
            case Serial:
                std::cout << "[FW-LOG-DEBUG]: Initializing serial connection for asio stuff in network handler." << std::endl;
                return;
            case UDP:
                std::cout << "[FW-LOG-DEBUG]: Initializing UDP connection for asio stuff in network handler." << std::endl;
                try
                {
                    asio::ip::udp::resolver resolver(context);
                    asio::ip::udp::resolver::results_type end_points
                    = resolver.resolve(infrastructure.target, std::to_string(infrastructure.port));

                    connection = std::make_unique<UDPConnection>(
                                    context,
                                    udp_socket,
                                    in_messages,
                                    end_points
                    );
                }
                catch (std::exception& e)
                {
                    std::cout << "[FW-LOG-ERROR]: " << e.what() << std::endl;
                    return;
                }
                return;
            case TCP:
                std::cout << "[FW-LOG-DEBUG]: Initializing TCP connection for asio stuff in network handler." << std::endl;
                return;
        }
    }


} // FW