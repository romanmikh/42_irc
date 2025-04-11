#include "../include/Client.hpp"

// ************************************************************************** //
//                       Constructors & Desctructors                          //
// ************************************************************************** //

Client::Client(pollfd clientSocket)
{
	_nickname = "undefined";
	_username = "undefined";
	_socket = clientSocket;
	_isRegistered = false;
	_isIRCOp = false;
	msgBuffer = "";
}

Client::~Client() {}


// ************************************************************************** //
//                               Accessors                                    //
// ************************************************************************** //

pollfd	Client::getSocket() const { return (_socket); }

int	Client::getFd() const { return (_socket.fd); }

std::string	Client::username() const { return (_username); }

std::string	Client::nickname() const { return (_nickname); }

std::string	Client::hostname() const { return (_hostname); }

void	Client::setFullName(std::string &fullname) { _fullname = fullname; }

void	Client::setNickname(std::string &nickname)
{
	_nickname = nickname;
	info("Client " + this->nickname() + "'s nickname was set to "	+ nickname);
}

void	Client::setUsername(std::string &username) { _username = username; }

void	Client::setHostname(std::string &hostname) { _hostname = hostname; }

void	Client::setIP(std::string IP) { _IP = IP; }

void	Client::setIRCOp(bool status) { _isIRCOp = status; }

void	Client::setRegistered(bool status) { _isRegistered = status; }

bool	Client::isRegistered() const { return (_isRegistered); }

bool	Client::isIRCOp() const { return _isIRCOp; }

std::vector<Channel*>&	Client::getClientChannels() { return (_clientChannels); }

std::vector<std::string>	Client::getChannelInvites() const { return (_clientChannelInvites); }

void	Client::addChannelInvite(const std::string& channelName)
{
	_clientChannelInvites.push_back(channelName);
}

void	Client::delChannelInvite(const std::string& channelName)
{
	std::vector<std::string>::iterator it = std::find(_clientChannelInvites.begin(), 
										_clientChannelInvites.end(), channelName);
	if (it != _clientChannelInvites.end())
		_clientChannelInvites.erase(it);
}


// ************************************************************************** //
//                             Public Functions                               //
// ************************************************************************** //

void	Client::popClientChannel(ChannelManager& manager,std::string channelName)
{
	Channel* chan = manager.getChanByName(channelName);
	std::vector<Channel*>::iterator it = std::find(_clientChannels.begin(), 
												   _clientChannels.end(), chan);
	if (it != _clientChannels.end())
		_clientChannels.erase(it);
	info(_nickname + " has left channel " + channelName);
}

void	Client::assignUserData(std::string &username, std::string &hostname, std::string &IP, std::string &fullName)
{
	setUsername(username);
	setHostname(hostname);
	setIP(IP);
	setFullName(fullName);

	sendMSG(this->getFd(), RPL_WELCOME((*this)));
	sendMSG(this->getFd(), RPL_YOURHOST((*this)));
	sendMSG(this->getFd(), RPL_CREATED((*this)));
	sendMSG(this->getFd(), RPL_MYINFO((*this)));
}

bool	Client::isInvited(const std::string& channelName) const
{
	std::vector<std::string>::const_iterator it = std::find(getChannelInvites().begin(), getChannelInvites().end(), channelName);
	return (it != getChannelInvites().end());
}
