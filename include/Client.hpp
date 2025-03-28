#pragma once
#include "irc.hpp"

class Client
{
	private:
		/* member variables */
		std::string	_username;
		std::string _nickname;
		pollfd		_socket;

		/* member functions */
        bool    _isNicknameValid(const std::string nickname) const;
        bool    _isUsernameValid(const std::string username) const;
        bool    _isOperator(void) const;

	public:
		/* construcotrs & destructors */
		Client();
		Client(pollfd &clientSocket);
		~Client();

		/* operator overloads */
        Client & operator = (const Client &other);
        
        /* accessors */
		int getFd();
		std::string getUsername();
		std::string getNickname();
		struct pollfd getSocket();
};
