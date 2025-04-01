#ifndef MSGHANDLER_HPP
#define MSGHANDLER_HPP
#include "irc.hpp"

class MsgHandler
{
	private:
		Server _server;

    public:
		MsgHandler(Server &server);
		~MsgHandler();

		void respond(std::string &msg, Client &client);
		bool receiveMessage(Client &client);
		void sendWelcomeProtocol(Client &client);
		void replyUSER(std::string &msg, Client &client);
		void handleNICK(std::string &msg, Client &client);
		void replyPONG(Client &client);
		void handleOPER(std::string &nickname, std::string &password, Client &client);
};

#endif