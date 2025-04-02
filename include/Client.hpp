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
		bool 		_isOperator;
		std::vector<std::string> _channels;

		
		public:
		/* construcotrs & destructors */
		Client(void);
		Client(pollfd &clientSocket);
		~Client(void);

		std::string msgBuffer;
        
        /* accessors */
		std::vector<std::string> getChannels() const;
		int 			getFd(void) const;
		struct pollfd 	getSocket(void) const;
		
		void			setIP(std::string IP);
		void 			setFullName(std::string &fullname);
		void 			setNickname(std::string &nickname);
		void 			setUsername(std::string &username);
		void 			setHostname(std::string &hostname);
		void 			setOperator(bool isOperator);
		
		std::string 	username(void) const;
		std::string 	nickname(void) const;
		std::string 	hostname(void) const;
        bool    		isOperator(void) const;
		
		/* member functions */
		void    		joinChannel(std::string channelName);
        void    		leaveChannel(std::string channelName);
		
		void 			setOper(bool status);

};
