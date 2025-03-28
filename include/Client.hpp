#pragma once
#include "irc.hpp"

class Client
{
	private:
		/* member variables */
		std::string	_username;
		std::string _nickname;
		std::string _fullname;
		std::string _hostname;
		std::string _IP;
		pollfd		_socket;

		/* member functions */
        bool    _isNicknameValid(const std::string nickname) const;
        bool    _isUsernameValid(const std::string username) const;
        bool    _isOperator(void) const;

	public:
		/* construcotrs & destructors */
		Client(void);
		Client(pollfd &clientSocket);
		~Client(void);
        
        /* accessors */
		int 			getFd(void) const;
		std::string 	getUsername(void) const;
		std::string 	getNickname(void) const;
		struct pollfd 	getSocket(void) const;
		
		void			setIP(std::string IP);
		void 			setFullName(std::string fullname);
		void 			setNickname(std::string nickname);
		void 			setUsername(std::string username);
		void 			setHostname(std::string hostname);

};
