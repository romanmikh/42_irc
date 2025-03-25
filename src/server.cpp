/*
// Example of accepting connections & reading data from client using a socket
// from https://reactive.so/post/42-a-comprehensive-guide-to-ft_irc/

// can be run as a standalone file outside of the project

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <stdio.h>

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;  // create two instances of type struct sockaddr_in from <netinet/in.h>
    socklen_t client_addr_len = sizeof(client_addr);
    char buffer[1024];
    int port = 6667; // Default IRC port

    // Create the server socket
    
    // int socket(int domain, int type, int protocol);
    //     domain:     AF_INET for IPv4
    //     type:       SOCK_STREAM for TCP
    //     protocol:   0 for default protocol for domain and type

    //     creates TCP socket (endpoint for server to listen for client connections)
    //     returns a file descriptor for the socket to bind, listen, accept, read/write connections
    
    server_fd = socket(AF_INET, SOCK_STREAM, 0); // int socket(int domain, int type, int protocol);
    if (server_fd < 0) {
        perror("socket!");
        return 1;
    }

    // Hold address info
    memset(&server_addr, 0, sizeof(server_addr));  // fill the server_addr with 0s
    server_addr.sin_family = AF_INET; // Set the address family to IPv4, from #include <netinet/in.h>
    server_addr.sin_addr.s_addr = INADDR_ANY; // Bind to all available network interfaces (localhost, LAN, IP). What we discussed on first call
    server_addr.sin_port = htons(port); // Convert the port to network byte order (networks use big-endian)

    // Bind the socket to the address and port

    // int bind(int socket, const struct sockaddr *address, socklen_t address_len);
    //     socket:         file descriptor returned by socket()
    //     address:        pointer to a struct sockaddr_in with the address and port to bind to
    //     address_len:    size of the address struct

    //     assigns address to unnamed socket (since socket initially identified only by address family)
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind!");
        close(server_fd);// if failure, release server socket, free port
        return 1;
    }

    // Listen for incoming connections
    
    // int     listen(int socket, int backlog);
    //     socket:     file descriptor returned by socket()
    //     backlog:    maximum length of the queue of pending connections
    //                 more than 5 people trying to connect at once will be rejected

    //     marks socket as ready to accept incoming connections
    
    if (listen(server_fd, 5) < 0) {
        perror("listen!");
        close(server_fd);
        return 1;
    }

    std::cout << "Server listening on port " << port << std::endl;

    // Accept a connection
    
    // int accept (int socket, struct sockaddr *address, socklen_t *address_len);
    //     socket:     file descriptor returned by socket()
    //     address:    pointer to a struct sockaddr_in to store the client address
    //     address_len: size of the address struct

    //     creates and returns a new file descriptor for the client socket
    
    client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
    if (client_fd < 0) {
        perror("accept!");
        close(server_fd);
        return 1;
    }

    std::cout << "Client connected" << std::endl;

    // Read data from the client
    
    // ssize_t read(int fd, void *buf, size_t count);
    //     fd:     file descriptor returned by accept()
    //     buf:    buffer to store the data
    //     count:  maximum number of bytes to read

    //     retrieves data sent by client, stores it in buffer
    //     returns the number of bytes read, or -1 on error
    //     -------------------------------------------------
    //     by default, read blocks until data is available

    //     #include <fcntl.h>
    //     fcntl(client_fd, F_SETFL, O_NONBLOCK);

    //     ^ this would make read return (-1) immediately if no data is available
    

    ssize_t bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);
    if (bytes_read < 0) {
        perror("read!");
        close(client_fd);
        close(server_fd);
        return 1;
    }

    buffer[bytes_read] = '\0'; // Null-terminate the buffer
    std::cout << "Received message: " << buffer << std::endl;
    // CAP LS 302 is the default message
   
    // while (true) {
    //     // /connect localhost 6667 <-- into irssi
    //     // /connect and /quote only will work, /nick /join /msg will need to be implemented server-side
    //     ssize_t bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);
    //     if (bytes_read < 0) {
    //         perror("read!");
    //         break;
    //     } else if (bytes_read == 0) {
    //         std::cout << "Client disconnected" << std::endl;
    //         break;
    //     }
    //     buffer[bytes_read] = '\0'; // Null-terminate the buffer
    //     std::cout << "Received message: " << buffer << std::endl;
    // }
    

    // Close the client and server sockets
    close(client_fd);
    close(server_fd);

    return 0;
}
*/