#include "UDPSocket.h"

namespace FW
{

    bool UDPSocket::Start()
    {
#ifdef _WIN32


        WSAData wsa_data;
        int res = WSAStartup(MAKEWORD(2, 2), &wsa_data);
        if (res != 0)
        {
            std::cout << "[FW-LOG-ERROR]: WSAStartup failed: " << res << std::endl;
            return false;
        }

#endif

        //Creating udp datagram socket
        udp_socket = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
        if(udp_socket < 0)
        {
            std::cout << "[FW-LOG-ERROR]: The udp socket could not initialized. " << std::endl;
            return false;
        }

        sockaddr_in socket_address{};
        memset(&socket_address, 0, sizeof(socket_address));
        socket_address.sin_family = AF_INET;
        socket_address.sin_addr.s_addr = inet_addr(target_ipv4.c_str());
        socket_address.sin_port = htons(rx_port);

        if(bind(udp_socket, (struct sockaddr*)(&socket_address), sizeof(struct sockaddr)))
        {
            std::cout << "[FW-LOG-ERROR]: The udp socket could not be bound into name. " << std::endl;
            int close_result = close_socket();
            if (close_result)
            {
                std::cout << "[FW-LOG-ERROR]: Error on port close. The error number: " << close_result << std::endl;
            }
            udp_socket = -1;
            return false;
        }

        std::cout << "[FW-LOG-DEBUG]: Listening to " << target_ipv4 << ":" << rx_port << std::endl;
        last_status.packet_rx_drop_count = 0;

        should_reader_thread_run = true;
		should_writer_thread_run = true;

        return IsOpen();
    }

    void UDPSocket::Close()
    {
        std::cout << "[FW-LOG-DEBUG]: Closing the UDP socket and finishes its associated threads."<< std::endl;
        int close_result = close_socket();
        if(close_result)
        {
           std::cout << "[FW-LOG-ERROR]: Error on port close. The error number: "<< close_result << std::endl;
        }
        should_reader_thread_run = false;
        should_writer_thread_run = false;
        
    }

    void UDPSocket::WriterThread()
    {
        do
        {
            while(out_queue.wait_for(1s) != std::cv_status::timeout)
            {
                //TODO: There is a bug related to writing a message to socket. FIX IT!!!!
                if(!out_queue.empty())
                {
                    mavlink_message_t message = out_queue.pop_front();
                    std::cout << "[]FW-LOG-INFO]: The sended message id " << message.msgid << std::endl;
                    uint8_t buffer_for_write[BUFFER_SIZE];
                    uint32_t length = mavlink_msg_to_send_buffer(buffer_for_write,&message);
                    int bytes_written = _write_port(buffer_for_write,length);
                    if(bytes_written < 0)
                    {
                        std::cout << "[FW-LOG-ERROR]: Could not write the message res = "
                        << bytes_written << " errno = " << errno << std::endl;
                        std::cout << "[FW-LOG-INFO]: Because of error during writing the port, the writer thread of udp"
                                     "socket will shut itself down. This can make program crash eventually. " << std::endl;
						should_writer_thread_run = false;
                        break;
                    }
                }
            }
        } while (should_writer_thread_run);
    }

    int UDPSocket::_write_port(uint8_t *buffer, uint32_t length)
    {
		std::scoped_lock lock(mut);

        if (!IsOpen())
        {
            return 0;
        }

        int bytes_written = 0;
        if(tx_port > 0)
        {
            struct sockaddr_in addr{};
            memset(&addr, 0, sizeof(addr));
            addr.sin_family = AF_INET;
            addr.sin_addr.s_addr = inet_addr(target_ipv4.c_str());
            addr.sin_port = htons(tx_port);
            bytes_written = sendto(udp_socket, (const char*)buffer, length, 0, (struct sockaddr*)&addr,  sizeof(struct sockaddr_in));
        }
        else
        {
            std::cout << "[FW-LOG-ERROR]: Sending before first packet received in udp socket! Must discover the system first!" << std::endl;
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
                    uint8_t v= cp;
                    std::cout << "[FW-LOG-ERROR][Dropout-Error]: " << v << std::endl;
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
						should_reader_thread_run = false;
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
            //reader thread should sleep to give time for writer thread (?)
			std::this_thread::sleep_for(READER_THREAD_INTERVAL);

        } while (should_reader_thread_run);
    }

    int UDPSocket::_read_port(uint8_t& cp)
    {
		std::scoped_lock lock(mut);

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
            socklen_t len = sizeof(sockaddr_in);
            result = recvfrom(udp_socket, (char*)&receiver_buffer, BUFFER_SIZE, 0, (sockaddr*)&addr, &len);
            if(tx_port < 0)
            {
                if(strcmp(inet_ntoa(addr.sin_addr), target_ipv4.c_str()) == 0)
                {
                    tx_port = ntohs(addr.sin_port);
                    std::cout << "[FW-LOG-DEBUG]: Got first packet, sending to " << target_ipv4 << ":" << rx_port << std::endl;
					std::cout << "[FW-LOG-DEBUG]: UDP Socket TX Port Set To: " << tx_port << std::endl;
                }
				else
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
    int UDPSocket::close_socket()
    {
    #ifdef _WIN32
            return closesocket(udp_socket);
    #else
            return close(udp_socket);
    #endif
    }
}

