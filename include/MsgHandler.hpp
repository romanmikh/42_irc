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

		void respond(std::string &msg, Client &client);
		void receiveMessage(Client &client);
		void sendWelcomeProtocol(Client &client);
		void replyUSER(std::string &msg, Client &client);

		void handleJOIN(std::string &channelName, Client &client);
		void handlePART(std::string &channelName, Client &client);
		void handleINVITE(std::string &username, std::string &channelName, Client &client);
		void handleKICK(std::string &msg, Client &client);
		void handleMODE(std::string &channelName, std::string &mode, Client &client);
		void handleTOPIC(std::string &channelName, std::string &topic, Client &client);
		void handlePASS(std::string &password, Client &client);
		void handleDIE(Client &client);
		void handleOPER(std::string &nickname, std::string &password, Client &client);
		void handleKILL(std::string &msg, Client &client);
		void handlePRIVMSG(std::string &msg, Client &client);
		
		Client *getClientByNick(std::string &nickname);
		bool			badPassword(std::string &message, Client &client);

};

#endif