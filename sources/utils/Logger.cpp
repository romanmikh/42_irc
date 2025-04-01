#include "../include/Logger.hpp"

// ************************************************************************** //
//                            Non-member Functions                            //
// ************************************************************************** //
void info(const std::string& message) {
    std::cout << GREEN << getCurrentTime() << " [INFO] " << message << RESET << std::endl;
}

void error(const std::string& message) {
    std::cerr << RED << getCurrentTime() << " [ERROR] " << message << RESET << std::endl;
}

void warning(const std::string& message) {
    std::cout << YELLOW << getCurrentTime() << " [WARNING] " << message << RESET << std::endl;
}