#include "UDPConnection.h"

namespace FW
{
    static std::vector<std::string> custom_split(const std::string& str, const char& separator)
    {
        std::vector<std::string> strings;
        int startIndex = 0, endIndex = 0;
        for (int i = 0; i <= str.size(); i++)
        {
            // If we reached the end of the word or the end of the input.
            if (str[i] == separator || i == str.size())
            {
                endIndex = i;
                std::string temp;
                temp.append(str, startIndex, endIndex - startIndex);
                strings.push_back(temp);
                startIndex = endIndex + 1;
            }
        }
        return strings;
    }

    void UDPConnection::ResolveEndPoint(const std::string& s)
    {
        //We are expecting host_name:port_number configuration
        std::vector<std::string> strings = custom_split(s,':');
        end_point = asio::ip::udp::endpoint
        (asio::ip::address::from_string(strings.at(0)), std::stoi(strings.at(1)));
    }

    bool UDPConnection::Discover(const std::string &s)
    {
        ResolveEndPoint(s);
    }

    void UDPConnection::Send(const mavlink_message_t &msg)
    {

    }

}

