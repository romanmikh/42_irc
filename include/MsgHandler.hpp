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
		void handleNICK(std::vector<std::string> &msgData, Client &client);
		void handleMODE(std::vector<std::string> &msgData, Client &client);
		void handlePART(std::vector<std::string> &msgData, Client &client);
		void handlePASS(std::vector<std::string> &msgData, Client &client);
		void handleOPER(std::vector<std::string> &msgData, Client &client);
		void handleJOIN(std::vector<std::string> &msgData, Client &client);
		void handleINVITE(std::vector<std::string> &msgData, Client &client);
		void handleUSER(std::string &msgData, Client &client);
		void handlePRIVMSG(std::string &msg, Client &client);

		void handleKICK(std::string &msg, Client &client);
		void handleTOPIC(std::string &msg, Client &client);
		void handleQUIT(std::string &msg, Client &client);
		void handleKILL(std::string &msg, Client &client);
		void handleDIE(Client &client);
};

#endif