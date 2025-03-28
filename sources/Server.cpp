#include "../include/Server.hpp"
#include "../include/Client.hpp"
#include "../include/Logger.hpp"

Server::Server(int port, std::string &passwd) 
{
	_port = port;
	_password = passwd;
	
	pollfd		listeningSocket;
	listeningSocket.fd = socket(AF_INET, SOCK_STREAM, 0);
	listeningSocket.events = POLLIN;
	listeningSocket.revents = 0;
	_sockets.push_back(listeningSocket);
	
	sockaddr_in	serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(_port);

	bind(_sockets[0].fd, (struct sockaddr *)(&serverAddr), sizeof(serverAddr));
	listen(_sockets[0].fd, 10);
	info("Listening...");
}

Server::~Server()
{
	for (unsigned int i = 0; i < _sockets.size(); i++)
		close(_sockets[i].fd);
}

void Server::sendWelcomeMessage(Client client)
{
	std::string msg = this->getName() + client.getNickname() + " 001 "
}

void Server::handleNewConnectionRequest()
{
	printStr("New connection request received", YELLOW);

	sockaddr_in		clientAddr;
	pollfd			clientSocket;
	unsigned int	addrLen = sizeof(clientAddr);
	
	_serverActivity--;
	clientSocket.events = POLLIN | POLLHUP | POLLERR;
	clientSocket.revents = 0;
	clientSocket.fd = accept(_sockets[0].fd, (sockaddr *)&clientAddr, &addrLen);
	if (clientSocket.fd < 0)
	{
		perror("New socket creation failed.\n");
		close(_sockets[0].fd);
		return ;
	}

	std::cout << "New client connected: " << clientSocket.fd << std::endl;
	Client newClient(clientSocket);
	_clients.insert(std::pair<int, Client>(clientSocket.fd, newClient));
	_sockets.push_back(newClient.getSocket());
	sendWelcomeMessage(newClient);
}

void Server::handleClientMessage(Client &client)
{
	char	buffer[1024];
	
	_serverActivity--;
	size_t bytes_read = read(client.getFd(), buffer, sizeof(buffer) - 1);
	if (bytes_read <= 0)
	{
		disconnectClient(client);
		return;
	}
	buffer[bytes_read] = '\0';
	std::cout << "Received message: " << buffer << std::endl;
}

void Server::disconnectClient(Client &client)
{
	std::cout << YELLOW << client.getUsername() << " disconnected!\n" << RESET;

	_serverActivity--;
	close(client.getFd());
	_clients.erase(client.getFd());
	for (unsigned int i = 0; i < _sockets.size(); i++) {
		if (_sockets[i].fd == client.getFd())
		{
			_sockets.erase(_sockets.begin() + i);
			break ;
		}
	}
}

void Server::run()
{
	info("Running...");
	while (1)
	{
		_serverActivity = poll(_sockets.data(), _sockets.size(), -1);
		if (_serverActivity > 0)
		{
			// if data is at listening socket 
			if (_sockets[0].revents & POLLIN)
				handleNewConnectionRequest();

			// check client sockets for data
			for (unsigned int i = 1; i < _sockets.size() && _serverActivity > 0; i++)
			{
				if (_sockets[i].revents & (POLLHUP | POLLERR | POLLNVAL))
				{
					disconnectClient(_clients[_sockets[i].fd]);
					i--;
				}
				else if (_sockets[i].revents & POLLIN)
					handleClientMessage(_clients[_sockets[i].fd]);
			}
		}
	}
}

// ************************************************************************** //
//                             Private Functions                              //
// ************************************************************************** //
pollfd Server::_makePollfd(int fd, short int events)
{
	struct pollfd pfd;
	pfd.fd = fd;
	pfd.events = events;
	return pfd;
}