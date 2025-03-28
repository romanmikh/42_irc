#include "../include/irc.hpp"
#include "../include/Server.hpp"
#include "../include/Logger.hpp"

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
//  list of clients on the server
//  list of channels
//  server password
//  

// Client class
//  username
//  nickname
//  fd for their socket
//   

// channel class/struct
//  list of operators/  channel mode
//  users in channel
//  

int main(int ac, char **av) {

    if (checkInput(ac, av) != 0)
        return 1;
    
    (void)ac;
    info("Starting server...");
    int port = atoi(av[1]);
    std::string passwd = av[2];

    Server server(port, passwd);
    server.run();
    return 0;
}