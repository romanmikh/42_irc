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

	public:
		Client();
		Client(pollfd &clientSocket);
		~Client();

		int getFd();
		std::string getUsername();
		std::string getNickname();
		struct pollfd getSocket();
};

#endif
