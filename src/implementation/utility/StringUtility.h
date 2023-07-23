#pragma once
#include <string>
#include <vector>

namespace FW
{

    class StringUtility
    {
    public:
        static std::vector<std::string> custom_split(const std::string& str, const char& separator);
    };

} // FW

