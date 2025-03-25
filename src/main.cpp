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
    printStr("Hello, World!", GREEN);

    // example usage of DynamicArray, will allow us to dynamically add new socket fds for
    // each new client without any extra work
    DynamicArray<pollfd> sockets(2);
    std::cout << "initial capacity = " << sockets.getCapacity() << std::endl;

    struct pollfd client1;
    client1.fd = socket(AF_INET, SOCK_STREAM, 0);
    
    struct pollfd client2;
    client2.fd = socket(AF_INET, SOCK_STREAM, 0);
    
    struct pollfd client3;
    client3.fd = socket(AF_INET, SOCK_STREAM, 0);

    sockets.append(client1);
    sockets.append(client2);
    sockets.append(client3);

    std::cout << "dynamic capacity = " << sockets.getCapacity() << std::endl;
    for (int i = 0; i < sockets.getCount(); i++)
        std::cout << "socket[i].fd = " << sockets[i].fd << std::endl;

    // this is how we can then use it with poll()
    poll(sockets.getData(), sockets.getCount(), 0);
    return 0;
}