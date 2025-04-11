#include "QuoteBot.hpp"
#include <netdb.h>

QuoteBot::QuoteBot()
{
	_apiSocketFd = -1;
	_isConnecting = false;
	_isConnected = false;
	_apiBuffer = "";
	_requesterClient = NULL;
	_requesterChannel = "";
}

QuoteBot::~QuoteBot(void)
{
	if (_apiSocketFd != -1)
		close(_apiSocketFd);
}

int	QuoteBot::getApiSocketFd(void) const { return _apiSocketFd; }

bool	QuoteBot::isConnecting(void) const { return _isConnecting; }

bool	QuoteBot::isConnected(void) const { return _isConnected; }

Client*	QuoteBot::getRequesterClient(void) const { return _requesterClient; }

std::string	QuoteBot::getRequesterChannel(void) const { return _requesterChannel; }

void	QuoteBot::setRequesterClient(Client* client)
{
	if (client)
		_requesterClient = client;
	else
		warning("QuoteBot::setRequesterClient() - client is NULL");
}

void	QuoteBot::setRequesterChannel(std::string channel)
{
	if (!channel.empty())
		_requesterChannel = channel;
	else
		warning("QuoteBot::setRequesterChannel() - channel is empty");
}

bool	QuoteBot::initiateConnection(Server& server)
{
	std::cout << PURPLE << "QuoteBot::initiateConnection()" << RESET << std::endl;

	if (_apiSocketFd != -1)
		return warning("QuoteBot is already connected"), false;

	const char* hostname = "api.forismatic.com";
	const char* port = "80";
	addrinfo hints, *res = NULL, *p = NULL;
	int socketFd = -1;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	int status = getaddrinfo(hostname, port, &hints, &res);

	std::cout << PURPLE << "getaddrinfo status: " << status << RESET << std::endl;
	if (status != 0)
	{
		warning(std::string("getaddrinfo: ") + gai_strerror(status));
		return false;
	}

	for (p = res; p != NULL; p = p->ai_next)
	{
		socketFd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (socketFd == -1)
		{
			warning("Failed to create socket for QuoteBot");
			continue;
		}
		if (fcntl(socketFd, F_SETFL, O_NONBLOCK) == -1)
		{
			warning("Failed to set socket to non-blocking mode");
			close(socketFd);
			socketFd = -1;
			continue;
		}
		status = connect(socketFd, p->ai_addr, p->ai_addrlen);
		if (status == -1 && errno == EINPROGRESS)
		{
			info("Connecting to QuoteBot API in fd " + intToString(socketFd));
			_apiSocketFd = socketFd;
			_isConnecting = true;
			pollfd apiPollFd = {_apiSocketFd, POLLIN, 0};
			server.addApiSocket(apiPollFd);
			break;
		}
		else
		{
			warning("QuoteBot connect() failed: " + std::string(strerror(errno)));
            close(socketFd);
            socketFd = -1;
		}
	}
	freeaddrinfo(res);
	return true;
}

void	QuoteBot::sendQuote(Server& server, std::string quote)
{
	if (_requesterClient == NULL)
		return warning("No requester client set for QuoteBot");
	if (_requesterChannel.empty())
		return warning("No requester channel set for QuoteBot");
	std::string message = "PRIVMSG " + _requesterChannel + " : " + quote + "\r\n";
	sendMSG(_requesterClient->getFd(), message);
	(void)server;
}

void	QuoteBot::processAPIResponse(Server& server)
{
	size_t pos = _apiBuffer.find("\r\n\r\n");
	std::string	quote = "";
	if (pos != std::string::npos)
	{
		quote = _apiBuffer.substr(pos + 4);
		_apiBuffer.erase(0, pos + 4);
	}
	else
		return warning("Incomplete response from QuoteBot API");
	sendQuote(server, quote);
}

void	QuoteBot::handleAPIMessage(Server& server)
{
	char	buffer[1024];
	while (true)
	{
		ssize_t bytes_read = recv(_apiSocketFd, buffer, sizeof(buffer) - 1, 0);
		if (bytes_read > 0)
		{
			buffer[bytes_read] = '\0';
			_apiBuffer += buffer;
		}
		else if (bytes_read == 0)
		{
			processAPIResponse(server);
			return;
		}
		else if (bytes_read < 0)
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK)
				break;
			else
			{
				// cleanupConnection(server, true);
				return;
			}
		}
	}
}
