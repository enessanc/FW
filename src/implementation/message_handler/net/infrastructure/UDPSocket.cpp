#include "UDPSocket.h"

namespace FW
{

    bool UDPSocket::Start()
    {
        //Creating udp datagram socket
        udp_socket = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
        if(udp_socket < 0)
        {
            std::cout << "[FW-LOG-ERROR]: The udp socket could not initialized. " << std::endl;
            return false;
        }

        sockaddr_in socket_address{};
        memset(&socket_address,0, sizeof(socket_address));
        socket_address.sin_family = AF_INET;
        socket_address.sin_addr.s_addr = inet_addr(target_ipv4.c_str());
        socket_address.sin_port = htons(rx_port);

        if(bind(udp_socket, (struct sockaddr*)(&socket_address), sizeof(struct sockaddr)))
        {
            std::cout << "[FW-LOG-ERROR]: The udp socket could not be bound into name. " << std::endl;
            close(udp_socket);
            udp_socket = -1;
            return false;
        }

        std::cout << "[FW-LOG-DEBUG]: Listening to " << target_ipv4 << ":" << rx_port << std::endl;
        last_status.packet_rx_drop_count = 0;

        is_open = true;

        return true;
    }

    void UDPSocket::Close()
    {
        if(is_open)
        {
            std::cout << "[FW-LOG-DEBUG]: Closing the UDP socket and finishes its associated threads."<< std::endl;
            int result = close(udp_socket);
            udp_socket = -1;
            if(result)
            {
                std::cout << "[FW-LOG-WARNING]: Error on port close. The error number: "<< result << std::endl;
            }
            is_open = false;
        }
    }

    void UDPSocket::WriterThread()
    {
        do
        {
            if(out_queue.wait_for(10000ms) != std::cv_status::timeout)
            {
                //TODO: There is a bug related to writing a message to socket. FIX IT!!!!
                while(!out_queue.empty())
                {
                    mavlink_message_t message = out_queue.front();
                    uint8_t buffer_for_write[BUFFER_SIZE];
                    uint32_t length = mavlink_msg_to_send_buffer(buffer_for_write,&message);
                    int bytes_written = _write_port(buffer_for_write,length);
                    if(bytes_written < 0)
                    {
                        std::cout << "[FW-LOG-ERROR]: Could not write the message res = "
                        << bytes_written << " errno = " << errno << std::endl;
                        std::cout << "[FW-LOG-INFO]: Because of error during writing the port, the writer thread of udp"
                                     "socket will shut itself down. This can be effect program crash eventually. " << std::endl;
                        is_open = false;
                        break;
                    }
                }
            }
        } while (is_open);
    }

    int UDPSocket::_write_port(uint8_t *buffer, uint32_t length)
    {
        int bytes_written = 0;
        if(tx_port > 0)
        {
            struct sockaddr_in addr{};
            memset(&addr, 0, sizeof(addr));
            addr.sin_family = AF_INET;
            addr.sin_addr.s_addr = inet_addr(target_ipv4.c_str());
            addr.sin_port = htons(tx_port);
            bytes_written = sendto(udp_socket, buffer, length, 0, (struct sockaddr*)&addr,  sizeof(struct sockaddr_in));
        }
        else
        {
            std::cout << "[FW-LOG-ERROR]:Sending before first packet received in udp socket! Must discover the system first!" << std::endl;
            bytes_written = -1;
        }
        return bytes_written;
    }

    void UDPSocket::ReaderThread()
    {
        do
        {
            uint8_t cp;
            mavlink_status_t status;
            uint8_t msgReceived = false;
            mavlink_message_t temp_message;

            int result = _read_port(cp);

            if (result > 0)
            {
                // the parsing
                msgReceived = mavlink_parse_char(MAVLINK_COMM_1, cp, &temp_message, &status);


                // check for dropped packets
                if ( (last_status.packet_rx_drop_count != status.packet_rx_drop_count) && debug )
                {
                    std::cout << "[FW-LOG-DEBUG]: Dropped " << status.packet_rx_drop_count << " Packets " << std::endl;
                    uint8_t v=cp;
                    std::cout << "[FW-LOG-DEBUG][Dropout-Error]: " << v << std::endl;
                }
                last_status = status;
            }

            // Couldn't read from port
            else
            {
                std::cout << "[FW-LOG-ERROR]: Could not read, result = " << result << "error no = " << errno << std::endl;
            }

            if(msgReceived)
            {
                AddToIncomingQueue(temp_message);
                if(debug)
                {
                    // Report info
                    std::cout << "Received message from UDP with ID: " << temp_message.msgid << "(sys:" << temp_message.sysid
                              << "|comp:" << temp_message.compid << ")" << std::endl;

                    std::cout << "Received UDP data: " << std::endl;
                    uint32_t i;
                    uint8_t buffer[MAVLINK_MAX_PACKET_LEN];

                    // check message is write length
                    unsigned int message_length = mavlink_msg_to_send_buffer(buffer, &temp_message);

                    // message length error
                    if (message_length > MAVLINK_MAX_PACKET_LEN)
                    {
                        std::cout << "[LOG-ERROR][FATAL-ERROR]: MESSAGE LENGTH IS LARGER THAN BUFFER SIZE"  <<
                        " Action: Ending the socket thread. That may cause crash eventually." << std::endl;
                        is_open = false;
                    }
                        // print out the buffer
                    else
                    {
                        for (i=0; i<message_length; i++)
                        {
                            unsigned char v=buffer[i];
                            std::cout << v;
                        }
                        std::cout << std::endl;
                    }
                }
            }

        } while (is_open);
    }

    int UDPSocket::_read_port(uint8_t& cp)
    {

        int result = -1;
        if(buff_ptr < buff_len)
        {
            cp=receiver_buffer[buff_ptr];
            buff_ptr++;
            result=1;
        }
        else
        {
            sockaddr_in addr{};
            socklen_t len = sizeof(struct sockaddr_in);
            result = recvfrom(udp_socket, &receiver_buffer, BUFFER_SIZE, 0, (struct sockaddr *)&addr, &len);
            if(tx_port < 0)
            {
                if(strcmp(inet_ntoa(addr.sin_addr), target_ipv4.c_str()) == 0)
                {
                    tx_port = ntohs(addr.sin_port);
                    std::cout << "[FW-LOG-DEBUG]: Got first packet, sending to " << target_ipv4 << ":" << rx_port << std::endl;
                }else
                {
                    std::cout << "[FW-LOG-ERROR]: Got packet from " << inet_ntoa(addr.sin_addr) << ":" << ntohs(addr.sin_port)
                    << " but listening on " << target_ipv4 << std::endl;
                }
            }
            if(result > 0)
            {
                buff_len=result;
                buff_ptr=0;
                cp=receiver_buffer[buff_ptr];
                buff_ptr++;
            }
        }
        return result;

    }
}

