#include "../include/Utils.hpp"
#include "../include/Logger.hpp"

// ************************************************************************** //
//                            Non-member Functions                            //
// ************************************************************************** //
void info(const std::string& message) {
    std::cout << getCurrentTime() << " [INFO] " << message << std::endl;
}

void error(const std::string& message) {
    std::cerr << RED << getCurrentTime() << " [ERROR] " << message << RESET << std::endl;
}

void warning(const std::string& message) {
    std::cout << YELLOW << getCurrentTime() << " [WARNING] " << message << RESET << std::endl;
}