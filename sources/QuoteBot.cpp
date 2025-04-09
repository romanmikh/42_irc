#include "QuoteBot.hpp"
#include <netdb.h>

QuoteBot::QuoteBot() : _apiSocketFd(-1), _isConnecting(false), _isSending(false), _isReceiving(false) {}

QuoteBot::~QuoteBot(void)
{
	if (_apiSocketFd != -1)
		close(_apiSocketFd);
}

bool	QuoteBot::inititateConnection(Server& server)
{
	if (_apiSocketFd != -1)
	{
		warning("QuoteBot is already connected");
		return false;
	}

	const char* hostname = "api.forismatic.com";
	const char* port = "80";
	addrinfo hints, *res = NULL, *p = NULL;
	int status;
	int socketFd = -1;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	int status = getaddrinfo(hostname, port, &hints, &res);
	if (status != 0)
	{
		warning(std::string("getaddrinfo: ") + gai_strerror(status));
		return;
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
			info("Connecting to QuoteBot API in fd " + std::to_string(socketFd));
			_apiSocketFd = socketFd;
			_isConnecting = true;
			pollfd apiPollFd = {_apiSocketFd, POLLIN, 0};
			server.addclient(apiPollFd);
			break;
		}
		else
		{
			warning("QuoteBot connect() falhou: " + std::string(strerror(errno)));
            close(socketFd);
            socketFd = -1;
		}
	}
	freeaddrinfo(res);
	return true;
}