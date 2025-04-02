#include "../include/irc.hpp"

// ************************************************************************** //
//                       Constructors & Desctructors                          //
// ************************************************************************** //
Server::Server(int port, std::string &passwd)
{
	_port = port;
	_password = passwd;
	_serverName = "42irc.local";
	
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

	for (size_t i = 0; i < _clients.size(); i++)
		delete &_clients[i];
}

// ************************************************************************** //
//                             Public Functions                               //
// ************************************************************************** //

std::string Server::name()
{
	return (_serverName);
}

void Server::addclient(pollfd &clientSocket)
{
	Client *newClient = new Client(clientSocket);
	_clients.insert(client_pair_t (clientSocket.fd, newClient));
	_sockets.push_back(newClient->getSocket());
}

void Server::handleNewConnectionRequest()
{
	info("New connection request received");

	sockaddr_in		clientAddr;
	pollfd			clientSocket;
	unsigned int	addrLen = sizeof(clientAddr);
	
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
	send(clientSocket.fd, "CAP * LS : \r\n", 13, 0);
	addclient(clientSocket);
  	info("New client connected with fd: " + intToString(clientSocket.fd));
}


void Server::disconnectClient(Client &client)
{
	info(client.username() + " disconnected");

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
	ChannelManager  manager;
	MsgHandler		msg(*this, manager);

	info("Running...");
	while (1)
	{
		int serverActivity = poll(_sockets.data(), _sockets.size(), -1);
		if (serverActivity > 0)
		{
			if (_sockets[0].revents & POLLIN)
			{
				handleNewConnectionRequest();
				serverActivity--;
			}
			for (unsigned int i = 1; i < _sockets.size() && serverActivity > 0;)
			{
				if (_sockets[i].revents & (POLLHUP | POLLERR | POLLNVAL))
				{
					disconnectClient(*(_clients[_sockets[i].fd]));
					serverActivity--;
				}
				else if (_sockets[i].revents & POLLIN)
				{
					bool clientDisconnected = msg.receiveMessage(*(_clients[_sockets[i].fd]));
					// added this check because we need to reset i when client disconnects inside this fn
					if (!clientDisconnected)
						i++;
					serverActivity--;
				}
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
//                  Unused (yet) helpful functions                            //
// ************************************************************************** //

std::vector<std::string> ftSplit(const std::string& input , char delim) {
	std::vector<std::string> result;
	std::stringstream ss(input);
	std::string item;
	while (std::getline(ss, item, delim) ) {
		result.push_back(item);
	}
	return result;
}

std::vector<std::string> splitByString(const std::string& input, const std::string& delim) {
    std::vector<std::string> result;
    size_t start = 0, end;
    while ((end = input.find(delim, start)) != std::string::npos) {
        result.push_back(input.substr(start, end - start));
        start = end + delim.length();
    }
    result.push_back(input.substr(start));
    return result;
}