#include "../include/Server.hpp"
#include "../include/Client.hpp"

Server::Server(int port, std::string &passwd) 
{
	pollfd listeningSocket;

	_port = port;
	_password = passwd;

	listeningSocket.fd = socket(AF_INET, SOCK_STREAM, 0);
	listeningSocket.events = POLLIN;
	listeningSocket.revents = 0;
	_sockets.push_back(listeningSocket);

	memset(&_serverAddr, 0, sizeof(_serverAddr));
    _serverAddr.sin_family = AF_INET;
    _serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    _serverAddr.sin_port = htons(_port);

	bind(_sockets[0].fd, (struct sockaddr *)(&_serverAddr), sizeof(_serverAddr));
	listen(_sockets[0].fd, 10);
	printStr("Listening...", YELLOW);
}

Server::~Server()
{
	for (unsigned int i = 0; i < _sockets.size(); i++)
		close(_sockets[i].fd);
}

void Server::handleNewConnectionRequest()
{
	printStr("New connection request received", YELLOW);

	sockaddr_in		clientAddr;
	pollfd			clientSocket;
	unsigned int	addrLen = sizeof(clientAddr);

	clientSocket.events = POLLIN | POLLHUP | POLLERR;
	clientSocket.revents = 0;
	clientSocket.fd = accept(_sockets[0].fd, (sockaddr *)&clientAddr, &addrLen);
	if (clientSocket.fd < 0)
	{
		close(_sockets[0].fd);
		perror("New socket creation failed.\n");
		return ;
	}

	Client newClient(clientSocket, clientAddr);
	_clients.insert(std::pair<int, Client>(clientSocket.fd, newClient));
	_sockets.push_back(clientSocket);
	std::cout << "New client connected: " << clientSocket.fd << std::endl;
}

void Server::handleClientMessage(Client &client)
{
	char	buffer[1024];
	
	size_t bytes_read = read(client.getFd(), buffer, sizeof(buffer) - 1);
	if (bytes_read <= 0)
	{
		disconnectClient(client.getFd());
		return;
	}
	buffer[bytes_read] = '\0';
	std::cout << "Received message: " << buffer << std::endl;
}

void Server::disconnectClient(int clientFd)
{
	std::cout << YELLOW << _clients[clientFd].getUsername() << " disconnected!\n" << RESET;

	close(clientFd);
	_clients.erase(clientFd);
	for (unsigned int i = 0; i < _sockets.size(); i++) {
		if (_sockets[i].fd == clientFd)
		{
			_sockets.erase(_sockets.begin() + i);
			break ;
		}
	}
}

void Server::run()
{
	printStr("Running...", YELLOW);
	while (1)
	{
		int socketActivity = poll(_sockets.data(), _sockets.size(), -1);
		if (socketActivity > 0)
		{
			// if data is at listening socket 
			if (_sockets[0].revents & POLLIN)
			{
				handleNewConnectionRequest();
				socketActivity--;
			}
			// check client sockets for data
			for (unsigned int i = 1; i < _sockets.size() && socketActivity > 0; i++)
			{
				if (_sockets[i].revents & (POLLHUP | POLLERR | POLLNVAL))
				{
					disconnectClient(_sockets[i].fd);
					socketActivity--;
					i--;
				}
				else if (_sockets[i].revents & POLLIN)
				{
					handleClientMessage(_clients[_sockets[i].fd]);
					socketActivity--;
				}
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