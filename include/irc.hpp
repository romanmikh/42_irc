#pragma once

#include "Utils.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"
#include "MsgHandler.hpp"
#include "Logger.hpp"
//#include "Manager.hpp"

/* Macros */
# define MAX_PORT 65535 // must be 16-bit unsigned integer
# define MIN_PORT 1024 // 1-1023 are reserved ports, require root

/* Enumerations */ 

/* Error messages */
# define ERR_USAGE "Usage: ./ircserv <port> <password>"
# define ERR_PORT "Invalid port number"
# define ERR_PORT_NUMERIC "Port number must be numeric (1024 - 65535)"
# define ERR_INVALID_PASSWORD "Invalid password"
# define ERR_INVALID_PASSWORD_LENGTH "Password must be 1 characters long"

// Reply codes for server responses
#define RPL_WELCOME "001"
#define RPL_YOURHOST "002"
#define RPL_CREATED "003"
#define RPL_MYINFO "004"
#define RPL_BOUNCE "005"

/* Structures */
typedef std::pair<int, Client *>	client_pair_t;
typedef std::map<int, Client *>		clients_t;

/* Function prototypes */ 
int     checkInput(int ac, char **av);
int     errMsgVal(int detail, const std::string& str, int code);
int     errMsg(const std::string& detail, const std::string& str, int code);
std::vector<std::string> split(const std::string& str, char delimiter);
