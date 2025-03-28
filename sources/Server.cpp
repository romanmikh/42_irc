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
	std::string msg = "001: " + client.getNickname() + ": Welcome to our " + _name + " server!\r\n";
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
	send(clientSocket.fd, "CAP * LS : \r\n", 13, 0);
	

	Client *newClient = new Client(clientSocket);
	_clients.insert(client_pair_t (clientSocket.fd, newClient));
	_sockets.push_back(newClient->getSocket());
  	info("New client connected with fd: " + intToString(clientSocket.fd));
}

void Server::handleUser(char *msg, Client &client)
{
	char **elems = ft_split(msg, ':');
	client.setFullName(elems[1]);

	char **moreNames = ft_split(elems[0], ' ');
	client.setUsername(moreNames[1]);
	client.setHostname(moreNames[2]);
	client.setIP(moreNames[3]);
	sendWelcomeMessage(client);
}

void Server::handleNick(char *nickname, Client &client)
{
	client.setNickname(nickname);
	info("Client " + client.getNickname() + " has set their nickname");
}

void Server::msgHandler(char *msg, Client &client)
{
	char **lines = ft_split_set(msg, (char *)"\r\n");

	std::cout << "Lines: " << lines[0] << std::endl;

	for (int i = 0; lines[i]; i++)
	{
		char **elems = ft_split(lines[i], ' ');
	//	if (ft_match(elems[0], "JOIN"))
	//		handleJoin();
		if (ft_match((const char *)elems[0], "USER"))
			handleUser(lines[i], client); 
		else if (ft_match((const char *)elems[0], "NICK"))
			handleNick(elems[1], client);
		// can add the rest later
		// can maybe use function pointers and loop to be more efficient ?
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
	std::cout << "Received message: " << buffer;

	// need to check for partial message here too.. can think about this later
	msgHandler(buffer, client); //-> this function can be part of manager if you want? can think about that later
	return (false);
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