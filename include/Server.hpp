#ifndef SERVER_HPP
#define SERVER_HPP
#include "irc.hpp"

class Client;

typedef std::pair<int, Client *>	client_pair_t;
typedef std::map<int, Client *>		clients_t;

class Server
{
	protected:

	private:
		std::string				_serverName;
		std::vector<pollfd>		_sockets;
		clients_t				_clients;
		std::string				_password;
		unsigned int			_port;

		std::map<std::string, std::string>	_opers;

		pollfd	_makePollfd(int fd, short int events, short int revents);

	public:
		Server(int port_num, std::string &passwd);
		~Server();

		void 			run();
		void 			parseOpersConfigFile(const char *file);
		void 			handleNewConnectionRequest();
		void 			disconnectClient(Client &client);
		void 			addclient(pollfd &clientSocket);
		bool			validatePassword(std::string &password);
		std::string 	name();
		
		std::map<std::string,std::string> getOpers();
		std::string	getPassword();

		//std::vector<std::string> ftSplit(const std::string& input , char delim);
		//std::vector<std::string> splitByString(const std::string& input, const std::string& delim);

};

#endif
