#include "Utils.hpp"

void	handleCtrlD(void)
{
	if (std::cin.eof())
    {
        std::cout << "\nCTRL+D detected. Exiting program..." << std::endl;
        std::exit(0);
    }
}

unsigned int getUnsignedInt(const std::string& prompt) {
    unsigned int value;
    std::cout << prompt;
    while (!(std::cin >> value)) {
        handleCtrlD();
        std::cout << "Invalid input. Enter a numeric value: ";
        std::cin.clear();
        std::cin.ignore(1000, '\n');
    }
    handleCtrlD();
    std::cin.ignore(1000, '\n');
    return value;
}

void    printStr(const std::string &text, const std::string &colour) {
   std::cout << colour << text << "\033[0m" << std::endl;
}

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