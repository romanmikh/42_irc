#include "./Client.hpp"

Client::Client()
{
	_nickname = "";
	_username = "";
}

Client::Client(pollfd &clientSocket, sockaddr_in &clientAddr)
{
	_nickname = "";
	_username = "";
	_socket = clientSocket;
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

std::string Client::getUsername()
{
	return (_username);
}
std::string Client::getNickname()
{
	return (_nickname);
}