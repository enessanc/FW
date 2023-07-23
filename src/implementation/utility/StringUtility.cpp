#include "StringUtility.h"

namespace FW
{
    std::vector<std::string> StringUtility::custom_split(const std::string& str, const char& separator)
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
} // FW