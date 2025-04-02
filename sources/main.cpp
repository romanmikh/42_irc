#include "../include/irc.hpp"

int    checkInput(int ac, char **av) {
    if (ac != 3) {
        return(errMsg("", ERR_USAGE, 1));
    }
    if (!isValidPort(av[1])) {
        return(errMsg("", ERR_PORT_NUMERIC, 1));
    }
    if (!isValidPassword(av[2])) {
        return(errMsg("", ERR_PASSWORD_FORMAT, 1));
    }
    // ...
    return 0;
}

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