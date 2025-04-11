#ifndef SERVER_HPP
#define SERVER_HPP
#include "irc.hpp"

class Client;

typedef std::pair<int, Client *>	client_pair_t;
typedef std::map<int, Client *>		clients_t;
#define SERVER_NAME std::string("42irc.local")

class Server
{
	protected:

	private:
		std::vector<pollfd>		_sockets;
		clients_t				_clients;
		std::string				_password;
		unsigned int			_port;
		static bool				_running;

		std::map<std::string, std::string>	_opers;

		pollfd	_makePollfd(int fd, short int events, short int revents);

	public:
		/* construcotrs & destructors */
		Server(int port_num, std::string &passwd);
		~Server(void);
		
		/* accessors*/
		std::map<std::string,std::string> 	getOpers(void);
		std::string							getPassword(void);
		clients_t&							getClients(void);
		Client*								getClientByUser(std::string& user) const;
		Client*								getClientByNick(std::string& nick) const;
		
		/* member functions*/
		void 			run(void);
		void 			parseOpersConfigFile(const char *file);
		void 			handleNewConnectionRequest();
		void			validatePassword(std::string &password, Client &client);
		void			validateIRCOp(std::string &nickname, std::string &password, Client &client);
		void 			addclient(pollfd &clientSocket);
		void 			disconnectClient(Client *client);
		void			shutdown();

		/* static members */
		static Server*  instance;
		static void 	SIGINTHandler(int signum);
	};

#endif
