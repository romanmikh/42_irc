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
		void handlePART(std::string &channelName, Client &client);
		void handleINVITE(std::string &username, std::string &channelName, Client &client);
		void handleKICK(std::string &username, std::string &channelName, Client &client);
		void handleMODE(std::string &channelName, std::string &mode, Client &client);
		void handleTOPIC(std::string &channelName, std::string &topic, Client &client);

};

#endif