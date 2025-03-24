#include "../inc/irc.hpp"

int errMsg(const std::string& detail, const std::string& str, int code)
{
    std::cerr << RED << "Error";
    if (!detail.empty())
        std::cerr << ": " << detail;
    if (!str.empty())
        std::cerr << ": " << str;
    std::cerr << RESET << std::endl;
    return code;
}

int errMsgVal(int detail, const std::string& str, int code)
{
    std::cerr << RED << "Error: " << detail << ": " << str << RESET << std::endl;
    return code;
}