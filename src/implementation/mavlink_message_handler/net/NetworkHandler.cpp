#include "NetworkHandler.h"
#include <iostream>

namespace FW
{

    bool NetworkHandler::Discover(const InfrastructureType &type, const std::string &host, const int &port)
    {
        bool result_of_discover = false;
        switch(type)
        {
            case Serial:
            {
                //TODO: Add serial port initialization here as a connection.
                return result_of_discover;
            }

            case UDP:
            {
                try
                {
                    asio::ip::udp::resolver resolver(context);
                    asio::ip::udp::resolver::results_type endpoint = resolver.resolve(host, std::to_string(port));

                    connection = std::make_unique<UDPConnection>(
                                    context,
                                    std::move(udp_socket),
                                    in_messages,
                                    endpoint
                    );
                    if(connection)
                    {
                        result_of_discover = connection->Discover();
                    }
                }
                catch (std::exception& e)
                {
                    //TODO: Replace this line with appropriate logging module
                    std::cerr << "[FW-LOG-ERROR]: " << e.what() << std::endl;
                    return result_of_discover;
                }
                return result_of_discover;
            }
            case TCP:
            {
                //TODO: Add TCP initialization here as a connection.
                return result_of_discover;
            }
            default:
            {
                std::cout << "[FW-LOG-ERROR]: The infrastructure type specified was not implemented." << std::endl;
                return result_of_discover;
            }
        }
    }

    bool NetworkHandler::Connect()
    {
        bool result_of_connection = false;
        if(connection)
        {
            try
            {
                result_of_connection = connection->Connect();
            }
            catch (std::exception& e)
            {
                //TODO: Replace this line with appropriate logging module
                std::cerr << "[FW-LOG-ERROR]: " << e.what() << std::endl;
                return result_of_connection;
            }
        }
        else
        {
            std::cout << "[FW-LOG-ERROR]: Before connecting a system, first you need to discover it." << std::endl;
        }

        return result_of_connection;
    }

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

        if(is_async_io_enabled)
        {
            context.stop();
            if (thread_for_context.joinable())
            {
                thread_for_context.join();
            }
            Connection* pointer_to_delete = connection.release();
            delete pointer_to_delete;
            is_async_io_enabled = false;
        }
    }

    bool NetworkHandler::IsConnected()
    {
        if(connection)
        {
            return connection->IsConnected();
        }
        return false;
    }

    void NetworkHandler::EnableAsyncIO()
    {
        if(connection)
        {
            thread_for_context = std::thread([this](){context.run();});
            is_async_io_enabled = true;
        }
        else
        {
            std::cout << "[FW-LOG-ERROR]: To disconnect, you first need to discover a system." << std::endl;
        }
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
} // FW