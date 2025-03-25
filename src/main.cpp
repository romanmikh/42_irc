#include "../inc/irc.hpp"
#include "../inc/DynamicArray.hpp"

int    checkInput(int ac, char **av) {
    if (ac != 3) {
        return(errMsg("", ERR_USAGE, 1));
    }
    if (!isValidPort(av[1])) {
        return(errMsg("", ERR_PORT_NUMERIC, 1));
    }
    if (strlen(av[2]) != 1) {
        return(errMsg("", ERR_INVALID_PASSWORD_LENGTH, 1));
    }
    // ...
    return 0;
}

int main(int ac, char **av) {

    if (checkInput(ac, av) != 0)
        return 1;
    printStr("Hello, World!", "G");

    // example usage of DynamicArray, will allow us to dynamically add new socket fds for
    // each new client without any extra work
    DynamicArray<pollfd> sockets(2);
    std::cout << "initial capacity = " << sockets.getCapacity() << std::endl;

    struct pollfd test;
    test.fd = 3;
    sockets.append(test);
    
    struct pollfd test1;
    test1.fd = 4;
    sockets.append(test1);
    
    struct pollfd test2;
    test2.fd = 5;
    sockets.append(test2);

    std::cout << "dynamic capacity = " << sockets.getCapacity() << std::endl;
    for (int i = 0; i < sockets.getCount(); i++)
        std::cout << "socket[i].fd = " << sockets[i].fd << std::endl;
    return 0;
}