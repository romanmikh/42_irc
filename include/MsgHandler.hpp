#ifndef MSGHANDLER_HPP
#define MSGHANDLER_HPP
#include "irc.hpp"
#include "Client.hpp"
#include "Server.hpp"

class MsgHandler
{
	private:
		Server _server;

    public:
		MsgHandler(Server &server);
		~MsgHandler();

		void replyUSER(std::string &msg, Client &client);
		void handleNICK(std::string &msg, Client &client);
		void respond(std::string &msg, Client &client);
		void replyPONG(Client &client);
		void sendWelcomeProtocol(Client &client);
		bool receiveMessage(Client &client);
};

#endif