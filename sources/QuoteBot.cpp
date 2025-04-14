#include "QuoteBot.hpp"
#include <netdb.h>

QuoteBot::QuoteBot()
{
	_apiSocketFd = -1;
	_botSocketFd = -1;
	_apiState = IDLE;
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

APIState	QuoteBot::apiState(void) const { return _apiState; }

Client*	QuoteBot::getRequesterClient(void) const { return _requesterClient; }

std::string	QuoteBot::getRequesterChannel(void) const { return _requesterChannel; }

void	QuoteBot::setBotSocketFd(int fd) { _botSocketFd = fd; }

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
	const char* hostname = "api.forismatic.com";
	const char* port = "80";
	addrinfo hints, *res = NULL, *p = NULL;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	int status = getaddrinfo(hostname, port, &hints, &res);
	if (status != 0)
		return warning(std::string("getaddrinfo: ") + gai_strerror(status)), false;

	int socketFd = -1;
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
			close(socketFd);
			socketFd = -1;
			continue;
		}

		status = connect(socketFd, p->ai_addr, p->ai_addrlen);
		if (status == 0)
		{
			info("Connected to QuoteBot API in fd " + intToString(socketFd));
			_apiSocketFd = socketFd;
			_apiState = SENDING;
			pollfd apiPollFd = {_apiSocketFd, POLLIN | POLLOUT, 0};
			server.addApiSocket(apiPollFd);
			break;
		}
		if (status == -1 && errno == EINPROGRESS)
		{
			info("Connecting to QuoteBot API in fd " + intToString(socketFd));
			_apiSocketFd = socketFd;
			_apiState = CONNECTING;
			pollfd apiPollFd = {_apiSocketFd, POLLIN | POLLOUT, 0};
			server.addApiSocket(apiPollFd);
			break;
		}
		else if (status == -1 && errno != EINPROGRESS)
		{
			warning("QuoteBot connect() failed: " + std::string(strerror(errno)));
			close(socketFd);
			socketFd = -1;
			_apiState = IDLE;
		}
	}
	freeaddrinfo(res);
	return (_apiSocketFd != -1);
}

void	QuoteBot::handleApiConnectionResult(Server& server)
{
	int errorCode;
	socklen_t	len = sizeof(errorCode);
	if (getsockopt(_apiSocketFd, SOL_SOCKET, SO_ERROR, &errorCode, &len) == -1)
		return closeApiConnection(server);
	if (errorCode == 0)
		_apiState = SENDING;
	else
		closeApiConnection(server);
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
			_apiState = COMPLETE;
			processAPIResponse(server);
			return;
		}
		else if (bytes_read < 0)
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK)
				break;
			else
			{
				closeApiConnection(server);
				return;
			}
		}
	}
}

void	QuoteBot::processAPIResponse(Server& server)
{
	std::string	responseOK = "HTTP/1.1 200 OK";
	if (_apiBuffer.compare(0, responseOK.size(), responseOK) != 0)
	{
		closeApiConnection(server);
		return warning("QuoteBot API returned an error");
	}
	size_t headerEnd = _apiBuffer.find("\r\n\r\n");
	if (headerEnd == std::string::npos)
		return closeApiConnection(server);
	std::string responseBody = _apiBuffer.substr(headerEnd + 4);
	_apiBuffer.clear();

	std::string decodeQuote = "";
	size_t	currentPos = 0;
	while (currentPos < responseBody.length())
	{
		size_t	chunkEnd = responseBody.find("\r\n", currentPos);
		if (chunkEnd == std::string::npos)
		{
			warning("Malformed chunk in API response");
			break;
		}

		std::string	chunkSizeHex = responseBody.substr(currentPos, chunkEnd - currentPos);
		char* endPtr;
		long chunkSize = strtol(chunkSizeHex.c_str(), &endPtr, 16);
		if (*endPtr != '\0' || chunkSize < 0)
		{
			warning("Malformed chunk in API response: invalid chunk size format");
			break;
		}
		if (chunkSize == 0)
			break;
		currentPos = chunkEnd + 2;
		if (currentPos + chunkSize > responseBody.length())
		{
			warning("Malformed chunk in API response: chunk size exceeds remaining data");
			break;
		}
		decodeQuote += responseBody.substr(currentPos, chunkSize);
		currentPos += chunkSize + 2;
	}
	if (!decodeQuote.empty())
	{
		size_t	lastChar = decodeQuote.find_last_not_of(" \t\n\r\f\v");
		if (std::string::npos != lastChar)
			decodeQuote.erase(lastChar + 1);
		else
			decodeQuote.clear();
		sendQuote(decodeQuote);
	}
	else
		warning("Malformed chunk in API response: empty quote");
	closeApiConnection(server);
}

void	QuoteBot::sendHttpRequest(Server& server)
{
	if (_apiSocketFd == -1)
		return closeApiConnection(server);
	std::string request = "GET /api/1.0/?method=getQuote&format=text&lang=en HTTP/1.1\r\n";
	request += "Host: api.forismatic.com\r\n";
	request += "User-Agent: ircserver\r\n";
	request += "Accept: */*\r\n";
	request += "Connection: close\r\n\r\n";
	if (send(_apiSocketFd, request.c_str(), request.length(), 0) == -1)
		return closeApiConnection(server);
	_apiState = RECEIVING;
}

void	QuoteBot::sendQuote(std::string quote)
{
	if (_requesterClient == NULL)
		return warning("No requester client set for QuoteBot");
	if (_requesterChannel.empty())
		return warning("No requester channel set for QuoteBot");

	std::string message = "PRIVMSG " + _requesterChannel + " :" + YELLOW + quote + RESET + "\r\n";
	if (send(_botSocketFd, message.c_str(), message.length(), MSG_DONTWAIT) == -1)
		warning("Failed to send message: " + std::string(strerror(errno)));
}

void	QuoteBot::closeApiConnection(Server& server)
{
	if (_apiSocketFd == -1)
	{
		_apiState = IDLE;
		_apiBuffer.clear();
		_requesterClient = NULL;
		_requesterChannel = "";
		return;
	}
	server.removeApiSocket(_apiSocketFd);
    close(_apiSocketFd);
	int	closedFd = _apiSocketFd;
    _apiSocketFd = -1;
    _apiState = IDLE;
    _apiBuffer.clear();
    _requesterClient = NULL;
    _requesterChannel = "";

    info("QuoteBot API connection cleanup complete for former fd: " + intToString(closedFd));
}