#ifndef QUOTE_BOT_HPP
#define QUOTE_BOT_HPP

#include "irc.hpp"

class Server;
class Client;

class QuoteBot
{
	private:
		int			_apiSocketFd;
		bool		_isConnecting;
		bool		_isConnected;
		Client*		_requesterClient;
		std::string	_apiBuffer;
		std::string	_requesterChannel;

	public:
		QuoteBot(void);
		~QuoteBot(void);

		bool		isConnecting(void) const;
		bool		isConnected(void) const;
		int			getApiSocketFd(void) const;
		Client*		getRequesterClient(void) const;
		std::string	getRequesterChannel(void) const;
		void		setRequesterClient(Client* client);
		void		setRequesterChannel(std::string channel);

		bool	initiateConnection(Server& server);
		void	sendQuote(Server& server, std::string quote);
		void	handleAPIMessage(Server& server);
		void	processAPIResponse(Server& server);
};

#endif