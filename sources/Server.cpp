#include "../include/Server.hpp"
#include "../include/Client.hpp"

Server::Server(int port, std::string &passwd) 
{
	_port = port;
	_password = passwd;
	_listeningSocket.fd = socket(AF_INET, SOCK_STREAM, 0);
	_sockets.push_back(_listeningSocket);

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

void Server::handleNewConnectionRequest()
{
	printStr("New connection request received", YELLOW);

	sockaddr_in		clientAddr;
	pollfd			clientSocket;
	unsigned int	addrLen = sizeof(clientAddr);
	
	clientSocket.events = POLLIN | POLLHUP;
	clientSocket.fd = accept(_listeningSocket.fd, (sockaddr *)&clientAddr, &addrLen);
	if (clientSocket.fd < 0)
	{
		perror("New socket creation failed.\n");
		return ;
	}

	Client newClient(clientSocket, clientAddr);
	_clients.push_back(newClient);
	_sockets.push_back(newClient.getSocket());

	std::cout << BLUE << "Num of sockets: " << _sockets.size() << RESET;
}

void Server::handleClientMessage(Client &client)
{
	char	buffer[1024];
	
	printStr("Client message received", YELLOW);
	size_t bytes_read = read(client.getFd(), buffer, sizeof(buffer) - 1);
	buffer[bytes_read] = '\0';
	std::cout << "Received message: " << buffer << std::endl;
}

void Server::disconnectClient(int i)
{
	std::cout << YELLOW << _clients[i].getUsername() << " disconnected!\n" << RESET;
	close(_sockets[i].fd);
	_sockets.erase(_sockets.begin() + i);
	_clients.erase(_clients.begin() + i);
}

void Server::run()
{
	printStr("Running...", YELLOW);
	while (1)
	{
		int socketActivity = poll(_sockets.data(), _sockets.size(), 0);
		if (socketActivity > 0)
		{
			// if data is at listening socket 
			if (_sockets[0].revents & POLLIN)
				handleNewConnectionRequest();
			
			// check client sockets for data
			for (unsigned int i = 1; i < _sockets.size(); i++)
			{
				if (_sockets[i].revents & POLLIN)
					handleClientMessage(_clients[i]);
				else if (_sockets[i].revents & POLLHUP)
					disconnectClient(i);	
			}
		}
	}
}
