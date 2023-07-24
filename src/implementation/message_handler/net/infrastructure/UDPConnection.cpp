#include "UDPConnection.h"

namespace FW
{

    bool UDPConnection::IsConnected()
    {
        return socket.is_open();
    }

    void UDPConnection::Disconnect()
    {
        if (IsConnected())
        {
            asio::post(context, [this]() {socket.close(); });
        }
    }

    void UDPConnection::Connect()
    {
        ReadMessage();
    }

    void UDPConnection::AddIncomingQueue()
    {
        messages_in.push_back(temp_msg_in);
        ReadMessage();
    }

    void UDPConnection::ReadMessage()
    {
        asio::async_read(socket, asio::buffer(&temp_msg_in, sizeof(mavlink_message_t)),
                         [this](std::error_code ec, std::size_t length)
                         {
                             if(!ec)
                             {
                                 if(temp_msg_in.len > 0)
                                 {
                                     AddIncomingQueue();
                                 }
                             }
                             else
                             {
                                 std::cout << "[FW-LOG-ERROR]: Error Reading Mavlink Message In UDP Connection\n"
                                              "Message: " << ec.message() << std::endl;
                                 socket.close();
                             }
                         });
    }

    void UDPConnection::Send(const mavlink_message_t &msg)
    {
        asio::post(context,
                   [this, msg]()
                   {
                       bool bWritingMessage = !messages_out.empty();
                       messages_out.push_back(msg);
                       if (!bWritingMessage)
                       {
                           WriteMessage();
                       }
                   }
        );
    }

    void UDPConnection::WriteMessage()
    {
        asio::async_write(socket, asio::buffer(&messages_out.front(), sizeof(mavlink_message_t)),
                          [this](std::error_code ec, std::size_t length)
                          {
                              if (!ec)
                              {
                                      messages_out.pop_front();
                                      if (!messages_out.empty())
                                      {
                                          WriteMessage();
                                      }
                                      else
                                      {
                                          return;
                                      }
                              }
                              else
                              {
                                  std::cout << "[FW-LOG-ERROR]: Error Writing Mavlink Message In UDP Connection\n"
                                               "Message: " << ec.message() << std::endl;
                                  socket.close();
                              }
                          }
        );
    }
}

