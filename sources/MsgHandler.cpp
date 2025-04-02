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
	if (chan->isClientChanOp(&client) || client.isIRCOp())
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
	if (chan->isClientChanOp(&client) || client.isIRCOp())
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
	if (chan->isClientChanOp(&client) || client.isIRCOp())
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
	if (chan->isClientChanOp(&client) || client.isIRCOp())
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
	sendMSG(client.getFd(), PONG);
}

void MsgHandler::replyBadPassword(Client &client)
{
	sendMSG(client.getFd(), RPL_PASSWDMISMATCH(client));
}

void	MsgHandler::handleOPER(std::string &nickname, std::string &password, Client &client)
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
		sendMSG(client.getFd(), RPL_YOUROPER(client));
	}
	else {
	 	replyBadPassword(client);
	}
}

void MsgHandler::handlePRIVMSG(std::string &msg, Client &client)
{
	std::string msgToSend = ":" + client.nickname() + "!" + client.username() + "@" + client.hostname() + " " + msg + "\r\n";
	Channel* chan = _manager.getChannels().at(client.getChannels()[0]);
	chan->broadcastToChannel(msgToSend, &client);
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
	else if (msgData[0] == "OPER")
		handleOPER(msgData[1], msgData[2], client);
	else if (msgData[0] == "PRIVMSG"){
		handlePRIVMSG(msg, client);
	}
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
	sendMSG(client.getFd(), RPL_WELCOME(client));
	sendMSG(client.getFd(), RPL_YOURHOST(client));
	sendMSG(client.getFd(), RPL_CREATED(client));
	sendMSG(client.getFd(), RPL_MYINFO(client));
}

void MsgHandler::sendMSG(int fd, std::string RPL)
{
	send(fd, RPL.c_str(), RPL.length(), MSG_DONTWAIT);
}