#include "../include/Client.hpp"

// ************************************************************************** //
//                       Constructors & Desctructors                          //
// ************************************************************************** //
Client::Client()
{
	_nickname = "default";
	_username = "default";
}

Client::Client(pollfd &clientSocket)
{
	_nickname = "default";
	_username = "default";
	_socket = clientSocket;
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

std::string Client::getUsername() const
{
	return (_username);
}

std::string Client::getNickname() const
{
	return (_nickname);
}

void Client::setFullName(std::string &fullname) {
	_fullname = fullname;
}

void Client::setNickname(std::string &nickname) {
	_nickname = nickname;
}

void Client::setUsername(std::string &username) {
	_username = username;
}

void Client::setHostname(std::string &hostname) {
	_hostname = hostname;
}

void Client::setIP(std::string IP) {
	_IP = IP;
}
// ************************************************************************** //
//                             Public Functions                               //
// ************************************************************************** //

// ************************************************************************** //
//                             Private Functions                              //
// ************************************************************************** //

// ************************************************************************** //
//                            Non-member Functions                            //
// ************************************************************************** //