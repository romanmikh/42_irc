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

void	MsgHandler::handleOPER(std::string &nickname, std::string &password, Client &client)
{
	std::map<std::string, std::string> allowedOpers = _server.getOpers();
	std::map<std::string, std::string>::iterator it = allowedOpers.find(nickname);

	if (it == allowedOpers.end())
	{
		sendMSG(client.getFd(), RPL_NOOPERHOST(client));
		return ;
	}
	if (it->second == password)
	{
		std::cout << nickname << " set as operator.\n";
		client.setIRCOp(true);
		sendMSG(client.getFd(), RPL_YOUROPER(client));
	}
	else {
		sendMSG(client.getFd(), RPL_PASSWDMISMATCH(client));
	}
}


void	MsgHandler::handlePASS(std::string &password, Client &client)
{
	if (password == _server.getPassword())
	{
		client.setRegistered(true);
		send(client.getFd(), "Client authenticated with the mserver\r\n", 23, MSG_DONTWAIT);
	}
	else
	{
		send(client.getFd(), "Invalid password\r\n", 18, MSG_DONTWAIT);
			_server.disconnectClient(client);
		}
	}

void MsgHandler::handlePRIVMSG(std::string &msg, Client &client)
{
	Channel* chan = _manager.getChannels().at(client.getChannels()[0]);
	chan->broadcastToChannel(CMD_STD_FMT(client) + " " + msg + "\r\n", &client);
}

void MsgHandler::respond(std::string &msg, Client &client)
{
	std::vector<std::string> msgData = split(msg, ' ');

	switch (getCommandType(msgData[0]))
	{
		case USER: replyUSER(msg, client);
			break ;
		case NICK: if (msgData.size() == 2) client.setNickname(msgData[1]);
			break ;
		case JOIN: if (msgData.size() > 1 && msgData[1][0] == '#') handleJOIN(msgData[1], client);
			break ;
		case PART: if (msgData.size() > 1 && msgData[1][0] == '#') handlePART(msgData[1], client);
			break ;
		case INVITE: if (msgData.size() > 1 && msgData[2][0] == '#') handleINVITE(msgData[1], msgData[2], client);
			break ;
		case KICK: if (msgData.size() > 1 && msgData[2][0] == '#') handleKICK(msgData[1], msgData[2], client);
			break ;
		case MODE: if (msgData.size() > 1 && msgData[1][0] == '#') handleMODE(msgData[1], msgData[2], client);
			break ;
		case TOPIC: if (msgData.size() > 1 && msgData[1][0] == '#') handleTOPIC(msgData[1], msgData[2], client);
			break ;
		case PING: sendMSG(client.getFd(), PONG);
			break ;
		case QUIT: _server.disconnectClient(client);
			break ;
		case OPER: if (msgData.size() == 3) handleOPER(msgData[1], msgData[2], client);
			break ;
		case PRIVMSG:handlePRIVMSG(msg, client);
			break ;
    	case PASS:handlePASS(msgData[1], client);
      		break ;
		case UNKNOWN:
			break ;
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

