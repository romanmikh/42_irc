#include "../include/Server.hpp"
#include "../include/Client.hpp"

Server::Server(int port, std::string &passwd) 
{
	_port = port;
	_password = passwd;
	_listeningSocket.fd = socket(AF_INET, SOCK_STREAM, 0);

	memset(&_serverAddr, 0, sizeof(_serverAddr));
    _serverAddr.sin_family = AF_INET;
    _serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    _serverAddr.sin_port = htons(_port);

	bind(_listeningSocket.fd, (struct sockaddr *)(&_serverAddr), sizeof(_serverAddr));
	listen(_listeningSocket.fd, 10);
	printStr("Listening...", YELLOW);
}

Server::~Server()
{
	close(_listeningSocket.fd);
}

void Server::run()
{
	char buffer[1024];

	printStr("Running...", YELLOW);
	while (1)
	{
		// if (poll(_sockets.data(), _sockets.size(), 0) > 0)
		// {
		// if data is at listening socket 
		Client newClient(_listeningSocket.fd);
		//_clients.push_back(newClient);
		//_sockets.push_back(newClient.getSocket());
		
		// // if client socket 
		// ssize_t bytes_read = read(newClient.getFd(), buffer, sizeof(buffer) - 1);  // -1 reserved for '\0' later
		// buffer[bytes_read] = '\0'; // Null-terminate the buffer
		// std::cout << "Received message: " << buffer << std::endl;
		// // CAP LS 302 is the default message

		while (true) {
			// /connect localhost 6667 <-- into irssi
			// /connect and /quote only will work, /nick /join /msg will need to be implemented server-side
			ssize_t bytes_read = read(newClient.getFd(), buffer, sizeof(buffer) - 1);  // -1 reserved for '\0' later
			if (bytes_read == 0) {
				std::cout << "Client disconnected" << std::endl;
				break;
			}
			buffer[bytes_read] = '\0'; // Null-terminate the buffer
			std::cout << "Received message: " << buffer << std::endl;
		}
			
		// }

	}
}
