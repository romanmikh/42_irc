#include "../include/irc.hpp"
#include <sstream>


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
void Server::sendWelcomeProtocol(Client &client)
{
	std::string msg1 = ":" + _serverName + " 001 " + client.nickname() + " :Welcome to the IRC Network, " + client.nickname() + "!" + client.username() + "@" + client.hostname() + "\r\n";
	send(client.getFd(), msg1.c_str(), msg1.length(), MSG_DONTWAIT);

	std::string msg2 = ":" + _serverName + " 002 " + client.nickname() + " :Your host is " + _serverName + ", running version 1.0\r\n";
	send(client.getFd(), msg2.c_str(), msg2.length(), MSG_DONTWAIT);
	
	std::string msg3 = ":" + _serverName + " 003 " + client.nickname() + " :This server was created, 2025-03-31\r\n";
	send(client.getFd(), msg3.c_str(), msg3.length(), MSG_DONTWAIT);
	
	std::string msg4 = ":" + _serverName + " 004 " + client.nickname() + _serverName + " 1.0 o itkol\r\n";
	send(client.getFd(), msg4.c_str(), msg4.length(), MSG_DONTWAIT);
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
	send(clientSocket.fd, "CAP * LS : \r\n", 13, 0);
	addclient(clientSocket);
  	info("New client connected with fd: " + intToString(clientSocket.fd));
}

void Server::replyUSER(std::string &msg, Client &client)
{
	std::vector<std::string> names = split(msg, ':');
	client.setFullName(names[1]);

	std::vector<std::string> moreNames = split(names[0], ' ');
	client.setUsername(moreNames[1]);
	client.setHostname(moreNames[2]);
	client.setIP(moreNames[3]);

	sendWelcomeProtocol(client);
}

void Server::handleNick(std::string &nickname, Client &client)
{
	client.setNickname(nickname);
	//info("Client " + client.nickname() + " has set their nickname");
}

void Server::replyPONG(Client &client)
{
	std::string pongMsg = "PONG " + _serverName + "\r\n";
	send(client.getFd(), pongMsg.c_str(), 6, 0);
}

void Server::msgHandler(char *msgBuffer, Client &client)
{
	std::istringstream ss(msgBuffer);
	std::string line;

	while (std::getline(ss, line))
	{
		std::vector<std::string> msgData = split(line, ' ');
		if (msgData[0] == "USER")
			replyUSER(line, client); 
		else if (msgData[0] == "NICK")
			handleNick(msgData[1], client);
		else if (msgData[0] == "QUIT")
			disconnectClient(client);
		else if (msgData[0] == "PING")
			replyPONG(client);
	}
}

bool Server::handleClientMessage(Client &client)
{
	char	buffer[1024];
	
	_serverActivity--;
	size_t bytes_read = read(client.getFd(), (void *)buffer, sizeof(buffer) - 1);
	if (bytes_read <= 0)
	{
		disconnectClient(client);
		return (true);
	}
	buffer[bytes_read] = '\0';
	std::cout << buffer; // only for testing

	// need to check for partial message here too.. can think about this later
	msgHandler(buffer, client); //-> this function can be part of manager if you want? can think about that later
	return (false);
}

void Server::disconnectClient(Client &client)
{
	info(client.username() + " disconnected");

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
	bool clientDisconnected;
	while (1)
	{
		_serverActivity = poll(_sockets.data(), _sockets.size(), -1);
		if (_serverActivity > 0)
		{
			if (_sockets[0].revents & POLLIN)
			{
				handleNewConnectionRequest();
			}
			for (unsigned int i = 1; i < _sockets.size() && _serverActivity > 0;)
			{
				if (_sockets[i].revents & (POLLHUP | POLLERR | POLLNVAL))
					disconnectClient(*(_clients[_sockets[i].fd]));
				else if (_sockets[i].revents & POLLIN)
				{
					// added this check because we need to reset i when client disconnects inside this fn
					clientDisconnected = handleClientMessage(*(_clients[_sockets[i].fd]));
					if (!clientDisconnected)
						i++;
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