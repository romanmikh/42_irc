#include "./Client.hpp"

Client::Client()
{
	_nickname = "default";
	_username = "default";
}

Client::Client(pollfd &clientSocket, sockaddr_in &clientAddr)
{
	_nickname = "default";
	_username = "default";
	_socket = clientSocket;
	_Addr = clientAddr;
}

Client::~Client() {}

pollfd Client::getSocket()
{
	return (_socket);
}

int Client::getFd()
{
	return (_socket.fd);
}

std::string Client::getUsername()
{
	return (_username);
}
std::string Client::getNickname()
{
	return (_nickname);
}