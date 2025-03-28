#ifndef SERVER_HPP
#define SERVER_HPP
#include "irc.hpp"

class Client;

class Server
{
	private:
		std::string				_name;
		std::vector<pollfd>		_sockets;
		std::map<int, Client>	_clients;
		//std::vector<Channel>	_channels;
		std::string				_password;
		unsigned int			_port;
		int						_serverActivity;

		/* Function Prototypes */
		pollfd	_makePollfd(int fd, short int events);


	public:
		Server(int port_num, std::string &passwd);
		~Server();

	//	std::string getName();
		void run();
		void handleNewConnectionRequest();
		void handleClientMessage(Client &client);
		void disconnectClient(Client &client);
		void sendWelcomeMessage(Client &client);
};

#endif
