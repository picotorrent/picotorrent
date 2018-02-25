#include "string.hpp"

using pt::String;

std::vector<std::string> String::Split(std::string const& value, std::string delim)
{
    std::vector<std::string> result;
    size_t offset = 0;
    std::string temp = value;

    do
    {
        size_t idx = temp.find(delim, 0);

        if (idx == std::string::npos)
        {
            result.push_back(temp);
            break;
        }

        result.push_back(temp.substr(0, idx));
        temp = temp.substr(idx + delim.size());
    } while (true);

    return result;
}
