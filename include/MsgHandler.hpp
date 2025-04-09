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
		void assignUserData(std::string &msg, Client &client);

		void handleNICK(std::vector<std::string> &msgData, Client &client);
		void handleMODE(std::string &msg, Client &client);
		void handleTOPIC(std::string &msg, Client &client);
		void validatePassword(std::string &password, Client &client);
		void handleDIE(Client &client);
		void validateIRCOp(std::string &nickname, std::string &password, Client &client);
		void handleKILL(std::string &msg, Client &client);
		void forwardPrivateMessage(std::string &msg, Client &client);
		
		bool badPassword(std::string &message, Client &client);

};

#endif