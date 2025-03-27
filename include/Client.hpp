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
		sockaddr_in	_Addr;

	public:
		Client(pollfd &clientSocket, sockaddr_in &clientAddr);
		~Client();

		int getFd();
		std::string getUsername();
		std::string getNickname();
		struct pollfd getSocket();
};

#endif
