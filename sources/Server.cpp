#include "../include/irc.hpp"

// ************************************************************************** //
//                       Constructors & Desctructors                          //
// ************************************************************************** //
Server::Server(int port, std::string &password)
{
	_port = port;
	_password = password;
	parseOpersConfigFile("./include/opers.config");
	
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

	int opt = 1;
	setsockopt(_sockets[0].fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	bind(_sockets[0].fd, (struct sockaddr *)(&serverAddr), sizeof(serverAddr));
	listen(_sockets[0].fd, 10);
}

Server::~Server()
{
	for (unsigned int i = 0; i < _sockets.size(); i++)
		close(_sockets[i].fd);

	for (clients_t::iterator it = _clients.begin(); it != _clients.end(); it++)
	{
		delete it->second;
	}
}

// ************************************************************************** //
//                               Accessors                                    //
// ************************************************************************** //
clients_t&	Server::getClients(void) { return (_clients); }

std::string Server::getPassword(void) { return (_password); }

std::map<std::string,std::string> Server::getOpers(void) { return (_opers); }

Client*	Server::getClientByUser(std::string& username) const
{
	for (clients_t::const_iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (it->second->username() == username)
			return (it->second);
	}
	return (NULL);
}

Client*	Server::getClientByNick(std::string& nickname) const
{
	for (clients_t::const_iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (it->second->nickname() == nickname)
		return (it->second);
	}
	warning("Client with nickname " + nickname + " not found");
	return (NULL);
}

// ************************************************************************** //
//                             Public Functions                               //
// ************************************************************************** //

void	Server::validatePassword(std::string &password, Client &client)
{
	if (password == getPassword())
	{
		client.setRegistered(true);
		sendMSG(client.getFd(), RPL_REGISTERED(client));
	}
	else
	{
		sendMSG(client.getFd(), ERR_PASSWDMISMATCH(client));
		disconnectClient(client);
	}
}

void	Server::validateIRCOp(std::vector<std::string> &msgData, Client &client)
{
	if (msgData.size() != 3) {
		return sendMSG(client.getFd(), ERR_NOSUCHCHANNEL(client, msgData[1]));
	}
	std::string nickname = msgData[1];
	std::string password = msgData[2];
	std::map<std::string, std::string> allowedOpers = getOpers();
	std::map<std::string, std::string>::iterator it = allowedOpers.find(nickname);

	if (it == allowedOpers.end()){
		return sendMSG(client.getFd(), ERR_NOOPERHOST(client));
	}
	if (it->second != password) {
		return sendMSG(client.getFd(), ERR_PASSWDMISMATCH(client));
	}
	info(client.nickname() + " set as operator");
	client.setIRCOp(true);
	sendMSG(client.getFd(), RPL_YOUROPER(client));
}

void	Server::shutdown()
{
	_running = false;
}

void	Server::parseOpersConfigFile(const char *fileName)
{
	std::ifstream file;
	
	file.open(fileName, std::ios::in);
	if (!file.is_open())
	{
		std::cerr << "Failed to open file: " << fileName << std::endl;
		return;
	}

	std::string line;
	while(std::getline(file, line))
	{
		std::vector<std::string> oper = split(line, ' ');
		_opers.insert(std::pair<std::string, std::string>(oper[0], oper[1]));
	}
}

void	Server::addclient(pollfd &clientSocket)
{
	Client *newClient = new Client(clientSocket);
	_clients.insert(client_pair_t (clientSocket.fd, newClient));
	_sockets.push_back(newClient->getSocket());
}

void	Server::handleNewConnectionRequest(void)
{
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
	sendMSG(clientSocket.fd, "CAP * LS : \r\n");
	addclient(clientSocket);
  	info("New client connected with fd: " + intToString(clientSocket.fd));
}


void	Server::disconnectClient(Client &client)
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

void	Server::SIGINTHandler(int signum)
{
	if (instance == NULL)
		return error("Server instance is NULL, cannot handle signal");
	if (signum == SIGINT)
	{
		info("SIGINT received, shutting server down...");
		_running = false;
	}
	for (clients_t::iterator it = instance->_clients.begin(); it != instance->_clients.end(); ++it) {
		Client &c = *it->second;
		sendMSG(c.getFd(), DIE(c));
	}
	instance->shutdown();
}

void	Server::run(void)
{
	ChannelManager  manager(*this);
	MsgHandler		msg(*this, manager);

	Server::instance = this;
	signal(SIGINT, SIGINTHandler);
	info("Running...");
	while (_running)
	{
		int serverActivity = poll(_sockets.data(), _sockets.size(), -1);
		if (serverActivity > 0)
		{
			if (_sockets[0].revents & POLLIN)
			{
				handleNewConnectionRequest();
				serverActivity--;
			}
			for (unsigned int i = _sockets.size() - 1; i > 0 && serverActivity > 0; --i)
			{
				if (_sockets[i].revents & (POLLHUP | POLLERR | POLLNVAL))
				{
					disconnectClient(*(_clients[_sockets[i].fd]));
					serverActivity--;
				}
				else if (_sockets[i].revents & POLLIN)
				{
					msg.receiveMessage(*(_clients[_sockets[i].fd]));
					serverActivity--;
				}
			}
		}
	}
}


// ************************************************************************** //
//                             Private Functions                              //
// ************************************************************************** //

pollfd	Server::_makePollfd(int fd, short int events, short int revents)
{
	struct pollfd pfd;
	pfd.fd = fd;
	pfd.events = events;
	pfd.revents = revents;
	return pfd;
}


// ************************************************************************** //
//                             Static Variables                               //
// ************************************************************************** //

Server*	Server::instance = NULL;
bool Server::_running = true;
