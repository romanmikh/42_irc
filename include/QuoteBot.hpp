#pragma once

#include "irc.hpp"

class QuoteBot
{
	private:
		int			_apiSocketFd;
		bool		_isConnecting;
		bool		_isSending;
		bool		_isReceiving;

	public:
		QuoteBot(void);
		~QuoteBot(void);

		bool	inititateConnection(Server& server);

};