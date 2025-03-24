#pragma once

#include "../inc/libft.hpp"

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

/* Structures */

/* Function prototypes */ 
int     checkInput(int ac, char **av);
int     errMsgVal(int detail, const std::string& str, int code);
int     errMsg(const std::string& detail, const std::string& str, int code);
