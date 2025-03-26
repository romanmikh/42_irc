#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include "irc.hpp"

class Client
{
	private:
		std::string	_username;
		std::string _nickname;
		pollfd		_socket;
		sockaddr_in	_clientAddr;

	public:
		Client(int listeningSocketFd);
		~Client();

		int getFd();
		struct pollfd getSocket();
};

#endif
