#include "../../include/irc.hpp"

std::string getCurrentTime(void) {
	std::time_t now = std::time(0);
	std::tm* localTime = std::localtime(&now);
	std::stringstream ss;
	ss << std::setfill('0')
	   << (1900 + localTime->tm_year) << "-"
	   << std::setw(2) << (1 + localTime->tm_mon) << "-"
	   << std::setw(2) << localTime->tm_mday << " "
	   << std::setw(2) << localTime->tm_hour << ":"
	   << std::setw(2) << localTime->tm_min << ":"
	   << std::setw(2) << localTime->tm_sec;
	return ss.str();
}

std::string intToString(int value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

std::string uintToString(unsigned int value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

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

std::vector<std::string> split(const std::string& str, char delimiter)
{
    std::vector<std::string> result;
    std::stringstream ss(str);
    std::string elem;

    while (std::getline(ss, elem, delimiter))
        result.push_back(elem);

    return result;
}

int isValidPassword(const std::string& pwd)
{
    for (std::string::const_iterator it = pwd.begin(); it != pwd.end(); ++it)
    {
        if (!std::isprint(*it))
            return (0);
    }
    return (pwd.length() >= 4 && pwd.length() <= 6);
}