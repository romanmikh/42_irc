#ifndef SERVER_HPP
#define SERVER_HPP
#include "irc.hpp"

class Client;

class Server
{
	private:
		std::vector<pollfd>		_sockets;
		std::map<int, Client>	_clients;
		//std::vector<Channel>	_channels;
		std::string				_password;
		unsigned int			_port;
		sockaddr_in				_serverAddr; // might not need this

		/* Function Prototypes */
		pollfd	_makePollfd(int fd, short int events);


	public:
		Server(int port_num, std::string &passwd);
		~Server();

		void run();
		void handleNewConnectionRequest();
		void handleClientMessage(Client &client, int i);
		void disconnectClient(int i);
};

#endif
