#include "../include/MsgHandler.hpp"

MsgHandler::MsgHandler(Server &server, ChannelManager &manager)
	: _server(server), _manager(manager) {};

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

void MsgHandler::handleJOIN(std::string &channelName, Client &client)
{
    _manager.joinChannel(client, channelName);
}

void MsgHandler::handlePART(std::string &channelName, Client &client)
{
	_manager.leaveChannel(client, channelName);
}

void MsgHandler::handleNICK(std::string &nickname, Client &client)
{
	client.setNickname(nickname);
}

void MsgHandler::handleINVITE(std::string &username, std::string &channel, Client &client)
{
	Channel* chan = _manager.getChannels().at(channel);
	if (chan->isClientOperator(&client) || client.isIRCOp())
	{
		info(client.username() + " invited " + username + " to channel " + channel);
		_manager.joinChannel(client, channel);
	}
	else {
		warning(client.username() + " is not an operator in channel " + channel);
	}
}

void MsgHandler::handleMODE(std::string &channel, std::string &mode, Client &client)
{
	Channel* chan = _manager.getChannels().at(channel);
	if (chan->isClientOperator(&client) || client.isIRCOp())
	{
		info(client.username() + " changed mode of channel " + channel + " to: " + mode);
		chan->setMode(mode);
	}
	else {
		warning(client.username() + " is not an operator in channel " + channel);
	}
}

void MsgHandler::handleTOPIC(std::string &channel, std::string &topic, Client &client)
{
	Channel* chan = _manager.getChannels().at(channel);
	if (chan->isClientOperator(&client) || client.isIRCOp())
	{
		info(client.username() + " changed topic of channel " + channel + " to: " + topic);
		chan->setTopic(topic);
	}
	else {
		warning(client.username() + " is not an operator in channel " + channel);
	}
}

void MsgHandler::handleKICK(std::string &username, std::string &channel, Client &client)
{
	Channel* chan = _manager.getChannels().at(channel);
	if (chan->isClientOperator(&client) || client.isIRCOp())
	{
		info(client.username() + " kicked " + username + " from channel " + channel);
		_manager.leaveChannel(client, channel);
	}
	else {
		warning(client.username() + " is not an operator in channel " + channel);
	}
}

void MsgHandler::replyPONG(Client &client)
{
	std::string pongMsg = "PONG " + SERVER_NAME + "\r\n";
	send(client.getFd(), pongMsg.c_str(), 6, 0);
}

void MsgHandler::replyBadPassword(Client &client)
{
	std::string msg = ":" + SERVER_NAME + " 464 " + client.nickname() + " :Password incorrect\r\n"; 
	send(client.getFd(), msg.c_str(), msg.length(), MSG_DONTWAIT);
}

void MsgHandler::handleOPER(std::string &nickname, std::string &password, Client &client)
{
	std::map<std::string, std::string>	allowedOpers = _server.getOpers();

	std::map<std::string, std::string>::iterator it = allowedOpers.find(nickname);
	if (it == allowedOpers.end()){
		// send 491 
		// return
	}
	if (it->second == password)
	{
		std::cout << nickname << " set as operator.\n";
		client.setIRCOp(true);
		std::string rpl_youroper = ":" + SERVER_NAME + " 381 " + client.nickname() + " :You are now an IRC operator\r\n";
		send(client.getFd(), rpl_youroper.c_str(), rpl_youroper.length(), MSG_DONTWAIT);
	}
	else {
	 	replyBadPassword(client);
	}
}

void MsgHandler::respond(std::string &msg, Client &client)
{
	std::vector<std::string> msgData = split(msg, ' ');
	if (msgData[0] == "USER")
		replyUSER(msg, client); 
	else if (msgData[0] == "NICK")
		handleNICK(msgData[1], client);
	else if (msgData[0] == "JOIN" && msgData.size() > 1 && msgData[1][0] == '#')
		handleJOIN(msgData[1], client);
	else if (msgData[0] == "PART" && msgData.size() > 1 && msgData[1][0] == '#')
		handlePART(msgData[1], client);
	else if (msgData[0] == "INVITE" && msgData.size() > 2 && msgData[2][0] == '#')
		handleINVITE(msgData[1], msgData[2], client);
	else if (msgData[0] == "KICK" && msgData.size() > 2 && msgData[2][0] == '#')
		handleKICK(msgData[1], msgData[2], client);
	else if (msgData[0] == "MODE" && msgData.size() > 2 && msgData[1][0] == '#')
		handleMODE(msgData[1], msgData[2], client);
	else if (msgData[0] == "TOPIC" && msgData.size() > 2 && msgData[1][0] == '#')
		handleTOPIC(msgData[1], msgData[2], client);
	else if (msgData[0] == "PING")
		replyPONG(client);
	else if (msgData[0] == "QUIT")
		_server.disconnectClient(client);
	//else if (msgData[0] == "JOIN")
		//add to channel ...
	else if (msgData[0] == "OPER")
		handleOPER(msgData[1], msgData[2], client);
}

void MsgHandler::receiveMessage(Client &client)
{
	char		buffer[1024];
	
	ssize_t bytes_read = read(client.getFd(), buffer, sizeof(buffer) - 1);
	if (bytes_read <= 0)
	{
		_server.disconnectClient(client);
		return ;
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
}

void MsgHandler::sendWelcomeProtocol(Client &client)
{
	std::string rpl_welcome = ":" + SERVER_NAME + " 001 " + client.nickname() + " :Welcome to the IRC Network, " + client.nickname() + "!" + client.username() + "@" + client.hostname() + "\r\n";
	std::string rpl_yourhost = ":" + SERVER_NAME + " 002 " + client.nickname() + " :Your host is " + SERVER_NAME + ", running version 1.0\r\n";
	std::string rpl_created = ":" + SERVER_NAME + " 003 " + client.nickname() + " :This server was created, 2025-03-31\r\n";
	std::string rpl_myinfo = ":" + SERVER_NAME + " 004 " + client.nickname() + " " + SERVER_NAME + " 1.0 o itkol\r\n";
	
	send(client.getFd(), rpl_welcome.c_str(), rpl_welcome.length(), MSG_DONTWAIT);
	send(client.getFd(), rpl_yourhost.c_str(), rpl_yourhost.length(), MSG_DONTWAIT);
	send(client.getFd(), rpl_created.c_str(), rpl_created.length(), MSG_DONTWAIT);
	send(client.getFd(), rpl_myinfo.c_str(), rpl_myinfo.length(), MSG_DONTWAIT);
}
