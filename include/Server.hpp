#ifndef SERVER_HPP
#define SERVER_HPP
#include "irc.hpp"

class Client;
class Manager;

class Server
{
	private:
		/* Member variables */
		std::vector<pollfd>		_sockets;
		std::map<int, Client>	_clients;
		std::string				_password;
		unsigned int			_port;
		int						_serverActivity;

		/* Member functions */
		pollfd	_makePollfd(int fd, short int events, short int revents);

	public:
		/* Constructors & Destructors */
		Server(int port_num, std::string &passwd);
		~Server();

		/* Member variables */
		Manager *				manager;

		/* Member functions */
		void run();
		void handleNewConnectionRequest();
		void handleClientMessage(Client &client);
		void disconnectClient(Client &client);
};

#endif
