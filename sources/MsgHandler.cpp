#include "../include/MsgHandler.hpp"

MsgHandler::MsgHandler(Server &server) : _server(server) {};

MsgHandler::~MsgHandler() {};

void MsgHandler::replyUSER(std::string &msg, Client &client)
{
	std::vector<std::string> names = split(msg, ':');
	client.setFullName(names[1]);

	std::vector<std::string> moreNames = split(names[0], ' ');
	client.setUsername(moreNames[1]);
	client.setHostname(moreNames[2]);
	client.setIP(moreNames[3]);

	sendWelcomeProtocol(client);
}

void MsgHandler::handleNICK(std::string &nickname, Client &client)
{
	client.setNickname(nickname);
	//info("Client " + client.nickname() + " has set their nickname");
}

void MsgHandler::replyPONG(Client &client)
{
	std::string pongMsg = "PONG " + _server.name() + "\r\n";
	send(client.getFd(), pongMsg.c_str(), 6, 0);
}


void MsgHandler::handleOPER(std::string &nickname, std::string &password, Client &client)
{
	std::map<std::string, std::string>	allowedOpers = _server.getOpers();

	if (allowedOpers[nickname] == password)
	{
		std::cout << nickname << "set as operator.\n";
		client.setOper(true);
	}
}

void MsgHandler::respond(std::string &msg, Client &client)
{
	std::vector<std::string> msgData = split(msg, ' ');
	if (msgData[0] == "USER")
		replyUSER(msg, client); 
	else if (msgData[0] == "NICK")
		handleNICK(msgData[1], client);
	else if (msgData[0] == "PING")
		replyPONG(client);
	else if (msgData[0] == "QUIT")
		_server.disconnectClient(client);
	//else if (msgData[0] == "JOIN")
		//add to channel ...
	else if (msgData[0] == "OPER")
		handleOPER(msgData[1], msgData[2], client);
}

bool MsgHandler::receiveMessage(Client &client)
{
	char		buffer[1024];
	
	ssize_t bytes_read = read(client.getFd(), buffer, sizeof(buffer) - 1);
	if (bytes_read <= 0)
	{
		_server.disconnectClient(client);
		return (true);
	}
	buffer[bytes_read] = '\0';
	std::cout << buffer; // only for testing

	client.msgBuffer += buffer;	
	size_t i;
	while ((i = client.msgBuffer.find("\r\n")) != std::string::npos)
	{
		std::string message = client.msgBuffer.substr(0, i);
		client.msgBuffer.erase(0, i + 2);
		respond(message, client);
	}
	return (false);
}

void MsgHandler::sendWelcomeProtocol(Client &client)
{
	std::string rpl_welcome = ":" + _server.name() + " 001 " + client.nickname() + " :Welcome to the IRC Network, " + client.nickname() + "!" + client.username() + "@" + client.hostname() + "\r\n";
	std::string rpl_yourhost = ":" + _server.name() + " 002 " + client.nickname() + " :Your host is " + _server.name() + ", running version 1.0\r\n";
	std::string rpl_created = ":" + _server.name() + " 003 " + client.nickname() + " :This server was created, 2025-03-31\r\n";
	std::string rpl_myinfo = ":" + _server.name() + " 004 " + client.nickname() + " " + _server.name() + " 1.0 o itkol\r\n";
	
	send(client.getFd(), rpl_welcome.c_str(), rpl_welcome.length(), MSG_DONTWAIT);
	send(client.getFd(), rpl_yourhost.c_str(), rpl_yourhost.length(), MSG_DONTWAIT);
	send(client.getFd(), rpl_created.c_str(), rpl_created.length(), MSG_DONTWAIT);
	send(client.getFd(), rpl_myinfo.c_str(), rpl_myinfo.length(), MSG_DONTWAIT);
}
