#ifndef SERVER_HPP
#define SERVER_HPP
#include "irc.hpp"

class Client;

class Server
{
	private:
		std::vector<pollfd>		_sockets;
		std::vector<Client>		_clients;
		//std::vector<Channel>	_channels;
		std::string				_password;
		unsigned int			_port;
		pollfd			_listeningSocket;
		sockaddr_in		_serverAddr;


	public:
		Server(int port_num, std::string &passwd);
		~Server();

		void run();
		void handleNewConnectionRequest();
		void handleClientMessage(Client &client);
		void disconnectClient(int i);
};

#endif
