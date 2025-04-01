#pragma once
#include "irc.hpp"

class Client
{
	private:
		std::string	_username;
		std::string _nickname;
		std::string _fullname;
		std::string _hostname;
		std::string _IP;
		pollfd		_socket;
		bool		_isOperator;
		
        // bool    _isNicknameValid(const std::string nickname) const;
        // bool    _isUsernameValid(const std::string username) const;
        // bool    _isOperator(void) const;
		
	public:
		Client(void);
		Client(pollfd &clientSocket);
		~Client(void);

		std::string msgBuffer;
        
		int 			getFd(void) const;
		struct pollfd 	getSocket(void) const;
		std::string 	username(void) const;
		std::string 	nickname(void) const;
		std::string 	hostname(void) const;
		
		void			setIP(std::string IP);
		void 			setFullName(std::string &fullname);
		void 			setNickname(std::string &nickname);
		void 			setUsername(std::string &username);
		void 			setHostname(std::string &hostname);
		void 			setOper(bool status);

};
