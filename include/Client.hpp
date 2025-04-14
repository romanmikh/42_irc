#pragma once
#include "irc.hpp"

class Channel;
class ChannelManager;

class Client
{
	private:
		std::string					_username;
		std::string 				_nickname;
		std::string 				_fullname;
		std::string 				_hostname;
		std::string 				_IP;
		pollfd						_socket;

		bool						_isRegistered;
		bool 						_isIRCOp;
		bool						_isBot;

		std::vector<Channel*>   	_clientChannels;
		std::vector<std::string>   	_clientChannelInvites;
		
	public:

		/* construcotrs & destructors */
		Client(pollfd clientSocket);
		~Client(void);

		std::string msgBuffer;
        
        /* accessors */
		std::vector<Channel*>&  	getClientChannels();
		std::vector<std::string> 	getChannelInvites() const;
		int 			getFd(void) const;
		struct pollfd 	getSocket(void) const;
		
		void			setIP(std::string IP);
		void 			setFullName(std::string &fullname);
		void 			setNickname(std::string &nickname);
		void 			setUsername(std::string &username);
		void 			setHostname(std::string &hostname);
		void			setRegistered(bool status);
		void 			setIRCOp(bool status);

		void			setBot(bool status);
		void			addChannelInvite(const std::string& channelName);
		void			delChannelInvite(const std::string& channelName);
		void			assignUserData(std::string &username, std::string &hostname, std::string &IP, std::string &fullName);

		std::string 	username(void) const;
		std::string 	nickname(void) const;
		std::string 	hostname(void) const;

		bool			isRegistered(void) const;
		bool 			isIRCOp(void) const;
		bool			isBot(void) const;
		bool 			isChanOp(const std::string &channelName, ChannelManager &manager) const;
        bool	        isInvited(const std::string& channelName) const;
		
		/* member functions */
		void    		joinChannel(ChannelManager& manager, std::string channelName);
        void    		popClientChannel(ChannelManager& manager, std::string channelName);
		

};
