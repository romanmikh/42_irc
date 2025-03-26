#include "libft.hpp"

int isDigits(const std::string& s)
{
    if (s.empty())
        return 0;
    for (std::string::const_iterator it = s.begin(); it != s.end(); ++it)
    {
        if (!std::isdigit(*it))
            return 0;
    }
    return 1;
}

int isValidPort(const std::string& s)
{
    if (s.empty())
        return 0;
    if (!isDigits(s))
        return 0;
    int port = std::atoi(s.c_str());
    return (port >= 0 && port <= 65535);
}