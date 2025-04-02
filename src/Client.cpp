#include "./Client.hpp"

Client::Client(int listeningSocketFd)
{
	_nickname = "";
	_username = "";
	socklen_t addrLen = sizeof(_clientAddr);
	_socket.fd = accept(listeningSocketFd, (sockaddr *)&_clientAddr, &addrLen);
}

Client::~Client()
{
	close(_socket.fd);
}

pollfd Client::getSocket()
{
	return (_socket);
}

int Client::getFd()
{
	return (_socket.fd);
}