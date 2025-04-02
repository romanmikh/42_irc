#include "../include/Client.hpp"

// ************************************************************************** //
//                       Constructors & Desctructors                          //
// ************************************************************************** //
Client::Client()
{
	_nickname = "default";
	_username = "default";
	_isOperator = false;
}

Client::Client(pollfd &clientSocket)
{
	_nickname = "default";
	_username = "default";
	_socket = clientSocket;
	_isOperator = false;
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
	// info("Client " + username() + "'s nickname was set to "	+ nickname);
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

bool Client::isOperator() const {
	return _isOperator;
}

void Client::setOperator(bool isOperator) {
	_isOperator = isOperator;
}

std::vector<std::string> Client::getChannels() const {
	return (_channels);
}
void 	Client::setOper(bool status)
{
	_isOperator = status;
}

// ************************************************************************** //
//                             Public Functions                               //
// ************************************************************************** //
void Client::joinChannel(std::string channelName) {
	_channels.push_back(channelName);
}

void Client::leaveChannel(std::string channelName) {
	std::vector<std::string>::iterator it = std::find(_channels.begin(), _channels.end(), channelName);
	if (it != _channels.end()) {
		_channels.erase(it);
	}
}

// ************************************************************************** //
//                             Private Functions                              //
// ************************************************************************** //

// ************************************************************************** //
//                            Non-member Functions                            //
// ************************************************************************** //