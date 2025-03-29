#ifndef SERVER_HPP
#define SERVER_HPP
#include "irc.hpp"

class Client;
class Manager;

class Server
{
	protected:

	public:

		/* Typedefs */
		typedef std::map<int, Client *>             clients_t;
		typedef std::pair<int, Client *>            client_pair_t;
		
		/* Constructors & Destructors */
		Server(int port_num, std::string &passwd);
		~Server();

		/* Member variables */
		Manager				manager();

		/* Member functions */
		void run();
		void handleNewConnectionRequest();
		bool handleClientMessage(Client &client);
		void disconnectClient(Client &client);
		void addclient(pollfd &clientSocket);
		void sendWelcomeMessage(Client &client);
		//std::vector<std::string> ftSplit(const std::string& input , char delim);
		//std::vector<std::string> splitByString(const std::string& input, const std::string& delim);


		// Message Handler
		void handleUser(std::string &msg, Client &client);
		void handleNick(std::string &msg, Client &client);
		void msgHandler(char *msg, Client &client);

		private:

		/* Member variables */
		std::string				_name;
		std::vector<pollfd>		_sockets;
		clients_t				_clients;
		std::string				_password;
		unsigned int			_port;
		int						_serverActivity;

		/* Member functions */
		pollfd	_makePollfd(int fd, short int events, short int revents);
};

#endif
