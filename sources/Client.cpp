#include "../include/Client.hpp"

// ************************************************************************** //
//                       Constructors & Desctructors                          //
// ************************************************************************** //
Client::Client()
{
	_nickname = "default";
	_username = "default";
	_isRegistered = false;
	_isIRCOp = true;

}

Client::Client(pollfd &clientSocket)
{
	_nickname = "default";
	_username = "default";
	_socket = clientSocket;
	_isRegistered = false;
	_isIRCOp = true;
}

Client::~Client() {}

// ************************************************************************** //
//                               Accessors                                    //
// ************************************************************************** //
pollfd Client::getSocket() const
{
	return (_socket);
}

int Client::getFd() const
{
	return (_socket.fd);
}

std::string Client::username() const
{
	return (_username);
}

std::string Client::nickname() const
{
	return (_nickname);
}

std::string Client::hostname() const
{
	return (_hostname);
}

void Client::setFullName(std::string &fullname)
{
	_fullname = fullname;
}

void Client::setNickname(std::string &nickname)
{
	_nickname = nickname;
	info("Client " + username() + "'s nickname was set to "	+ nickname);
}

void Client::setUsername(std::string &username)
{
	_username = username;
}

void Client::setHostname(std::string &hostname)
{
	_hostname = hostname;
}

void Client::setIP(std::string IP)
{
	_IP = IP;
}


void 	Client::setIRCOp(bool status)
{
	_isIRCOp = status;
}

void Client::setRegistered(bool status)
{
	_isRegistered = status;
}


bool Client::isRegistered() const
{
	return (_isRegistered);
}


bool Client::isIRCOp() const {
	return _isIRCOp;
}

std::vector<std::string> Client::getClientChannels() const {
	return (_clientChannels);
}

// ************************************************************************** //
//                             Public Functions                               //
// ************************************************************************** //
void Client::joinChannel(std::string channelName)
{
	_clientChannels.push_back(channelName);
}

void Client::leaveChannel(std::string channelName)
{
	std::vector<std::string>::iterator it = std::find(_clientChannels.begin(), 
											_clientChannels.end(), channelName);
	if (it != _clientChannels.end()) {
		_clientChannels.erase(it);
	}
}
