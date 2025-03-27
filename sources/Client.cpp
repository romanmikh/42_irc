#include "./Client.hpp"

Client::Client(int clientFd, sockaddr_in &clientAddr)
{
	_nickname = "";
	_username = "";
	_socket.fd = clientFd;
	_Addr = clientAddr;
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