#include "../include/irc.hpp"

// ************************************************************************** //
//                       Constructors & Desctructors                          //
// ************************************************************************** //
Server::Server(int port, std::string &passwd) 
{
	_port = port;
	_password = passwd;
	_name = "42IRC";
	
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

	manager = new Manager(*this);
}

Server::~Server()
{
	for (unsigned int i = 0; i < _sockets.size(); i++)
		close(_sockets[i].fd);

	for (size_t i = 0; i < _clients.size(); i++)
		delete &_clients[i];
}

// ************************************************************************** //
//                             Public Functions                               //
// ************************************************************************** //
void Server::sendWelcomeMessage(Client &client)
{
	std::string msg = "001 :" + client.getNickname() + ": Welcome to our " + _name + " server!\r\n";
	send(client.getFd(), msg.c_str(), msg.length(), MSG_DONTWAIT);
}

void Server::handleNewConnectionRequest()
{
	info("New connection request received");

	sockaddr_in		clientAddr;
	pollfd			clientSocket;
	unsigned int	addrLen = sizeof(clientAddr);
	
	_serverActivity--;
	clientSocket.fd = accept(_sockets[0].fd, (sockaddr *)&clientAddr, &addrLen);
	clientSocket = _makePollfd(clientSocket.fd, POLLIN | POLLHUP | POLLERR, 0);
	if (clientSocket.fd < 0)
	{
		error("New socket creation failed.");
		close(_sockets[0].fd);
		return ;
	}

	info("New connection request received");
	//if (validatePassword())
	send(clientSocket.fd, "CAP * LS : \r\n", 10, 0);

	// parse NICK and USER and use to construct client object
	// ...

	Client* newClient = new Client(clientSocket);
	_clients.insert(std::pair<int, Client>(clientSocket.fd, *newClient));
	_sockets.push_back(newClient->getSocket());
	sendWelcomeMessage(newClient);
  info("New client connected with fd: " + intToString(clientSocket.fd));
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
	std::cout << "Received message: " << buffer;
}

void Server::disconnectClient(Client &client)
{
	info(client.getUsername() + " disconnected");

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
			if (_sockets[0].revents & POLLIN)
			{
				handleNewConnectionRequest();
			}
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
pollfd Server::_makePollfd(int fd, short int events, short int revents)
{
	struct pollfd pfd;
	pfd.fd = fd;
	pfd.events = events;
	pfd.revents = revents;
	return pfd;
}

// ************************************************************************** //
//                            Non-member Functions                            //
// ************************************************************************** //