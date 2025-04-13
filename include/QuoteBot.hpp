#ifndef QUOTE_BOT_HPP
#define QUOTE_BOT_HPP

#include "irc.hpp"

class Server;
class Client;

enum APIState
{
	IDLE,
	CONNECTING,
	SENDING,
	RECEIVING,
	COMPLETE
};

class QuoteBot
{
	private:
		int			_apiSocketFd;
		int			_botSocketFd;
		APIState	_apiState;
		Client*		_requesterClient;
		std::string	_requesterChannel;
		std::string	_apiBuffer;

	public:
		QuoteBot(void);
		~QuoteBot(void);

		APIState	apiState(void) const;
		int			getApiSocketFd(void) const;
		Client*		getRequesterClient(void) const;
		std::string	getRequesterChannel(void) const;
		void		setBotSocketFd(int fd);
		void		setRequesterClient(Client* client);
		void		setRequesterChannel(std::string channel);

		bool	initiateConnection(Server& server);
		void	handleApiConnectionResult(Server& server);
		void	sendHttpRequest(Server& server);
		void	handleAPIMessage(Server& server);
		void	processAPIResponse(Server& server);
		void	sendQuote(std::string quote);
		void	closeApiConnection(Server& server);
};

#endif