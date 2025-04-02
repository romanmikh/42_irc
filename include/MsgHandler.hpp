#ifndef MSGHANDLER_HPP
#define MSGHANDLER_HPP
#include "irc.hpp"

class ChannelManager;

class MsgHandler
{
	private:
		Server& _server;
		ChannelManager& _manager;

    public:
		MsgHandler(Server& server, ChannelManager& _manager);
		~MsgHandler();

		void replyUSER(std::string &msg, Client &client);
		void handleNICK(std::string &msg, Client &client);
		void respond(std::string &msg, Client &client);
		void replyPONG(Client &client);
		void sendWelcomeProtocol(Client &client);
		bool receiveMessage(Client &client);
		void handleJOIN(std::string &channelName, Client &client);
};

#endif