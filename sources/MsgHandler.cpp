#include "../include/MsgHandler.hpp"

// ************************************************************************** //
//                       Constructors & Desctructors                          //
// ************************************************************************** //
MsgHandler::MsgHandler(Server &server, ChannelManager &manager)
										: _server(server), _manager(manager) {};

MsgHandler::~MsgHandler() {};


// ************************************************************************** //
//                             Public Functions                               //
// ************************************************************************** //
void	MsgHandler::replyUSER(std::string &msg, Client &client)

{
	std::vector<std::string> names = split(msg, ':');
	client.setFullName(names[1]);

	std::vector<std::string> moreNames = split(names[0], ' ');
	client.setUsername(moreNames[1]);
	client.setHostname(moreNames[2]);
	client.setIP(moreNames[3]);

	sendWelcomeProtocol(client);
}

// can we remove wrappers for single line function call and just call them directly inside respond() ??
void	MsgHandler::handleJOIN(std::string &channelName, Client &client)
{
    _manager.addToChannel(client, channelName);
}

void	MsgHandler::handlePART(std::string &channelName, Client &client)
{
	_manager.removeFromChannel(client, channelName);
}

void MsgHandler::handleINVITE(std::string &username, std::string &channelName, 
																Client &client)
{
  // need to send 482 ERR_CHANOPRIVSNEEDED
	_manager.inviteClient(username, channelName, client);
}

void MsgHandler::handleMODE(std::string &channelName, std::string &mode, 
																Client &client)
{
	Channel* chan = _manager.getChanByName(channelName);
	if (!chan) {
		return warning("Channel " + channelName + " does not exist");
	}
	if (chan->isClientChanOp(&client) || client.isIRCOp())
	{
		if (chan->actionMode(mode, client))
			return ;
		else
			return warning("Invalid mode: " + mode + ". +/- {i, t, k, o, l}");	
	}
  // need to send 482 ERR_CHANOPRIVSNEEDED
	else
		return warning(client.username() + " is not an operator in channel " + 
																channelName);
}

void MsgHandler::handleTOPIC(std::string &channelName, std::string &topic, 
																Client &client)
{
	Channel* chan = _manager.getChanByName(channelName);
	if (!chan) {
		return warning("Channel " + channelName + " does not exist");
	}
	if (chan->isClientChanOp(&client) || client.isIRCOp())
	{
		chan->setTopic(topic);
		info(client.username() + " changed topic of channel " + channelName + 
															   " to: " + topic);
	}
	else {
		warning(client.username() + " is not an operator in channel " + 
																channelName);
	}
}

void MsgHandler::handleKICK(std::string &username, std::string &channelName, 
																Client &client)
{
	Channel* chan = _manager.getChanByName(channelName);
	if (!chan) {
		return warning("Channel " + channelName + " does not exist");
	}
	if (chan->isClientChanOp(&client) || client.isIRCOp())
	{
		info(client.username() + " kicked " + username + " from channel " + 
																channelName);
		_manager.removeFromChannel(client, channelName);
	}
	else {
  // need to send 482 ERR_CHANOPRIVSNEEDED
		warning(client.username() + " is not an operator in channel " + 
																channelName);
	}
}

void	MsgHandler::handleOPER(std::string &nickname, std::string &password, 
																Client &client)
{
	std::map<std::string, std::string> allowedOpers = _server.getOpers();
	std::map<std::string, std::string>::iterator it = allowedOpers.find(nickname);

	if (it == allowedOpers.end())
	{
		sendMSG(client.getFd(), ERR_NOOPERHOST(client));
		return ;
	}
	if (it->second == password)
	{
		std::cout << nickname << " set as operator.\n";
		client.setIRCOp(true);
		sendMSG(client.getFd(), RPL_YOUROPER(client));
	}
	else {
		sendMSG(client.getFd(), ERR_PASSWDMISMATCH(client));
	}
}

void	MsgHandler::handlePASS(std::string &password, Client &client)
{
	if (password == _server.getPassword())
	{
		client.setRegistered(true);
		sendMSG(client.getFd(), RPL_REGISTERED(client));
	}
	else
	{
		// Needs to use RPL standard: 
		//send(client.getFd(), "Invalid password\r\n", strlen("Invalid password\r\n"), MSG_DONTWAIT);
		sendMSG(client.getFd(), RPL_PASSWDMISMATCH(client));
		_server.disconnectClient(client);
	}
}

void	MsgHandler::handlePRIVMSG(std::string &msg, Client &client)
{
	const std::vector<Channel*>& clientChannels = client.getClientChannels();
	std::map<std::string, Channel*> allChannels = _manager.getChannels();

	if (clientChannels.empty())
		return error("Client not in any channel, IRSSI applying PRIVMSG incorrectly");

	if (msg.empty())
        return warning("Empty PRIVMSG received");

    std::vector<std::string> tokens = split(msg, ' ');
    if (tokens.size() < 2 || tokens[0] != "PRIVMSG")
        return warning("Invalid PRIVMSG format");

    const std::string& channel = tokens[1];
    if (channel.empty() || channel[0] != '#') {
        return warning("PRIVMSG channel is missing or invalid");
	}

	std::map<std::string, Channel*>::iterator it = allChannels.find(channel);
	if (it == allChannels.end())
		return warning("Channel " + channel + " does not exist in the server");

	Channel* chan = it->second;
	if (chan->isEmpty())
		return warning("Channel is empty");

	chan->broadcastToChannel(CMD_STD_FMT(client) + " " + msg, &client);
}

void MsgHandler::handleKILL(std::string &msg, Client &killer)
{
	if (!killer.isIRCOp())
	{
		sendMSG(killer.getFd(), ERR_NOPRIVILAGES(killer));
		return ;
	}

	std::istringstream ss(msg);
	std::string killCommand, userToKill, reasonToKill;
	getline(ss, killCommand, ' ');
	getline(ss, userToKill, ' ');
	getline(ss, reasonToKill);

	clients_t &clients = _server.getClients();
	for (clients_t::iterator it = clients.begin(); it != clients.end(); it++)
	{
		Client *client = it->second;
		if (client->nickname() == userToKill)
		{
			// broadcast QUIT to all client's channels
			std::map<std::string, Channel*> allChannels = _manager.getChannels();
			std::vector<Channel*> clientChannels = client->getClientChannels();
			for (size_t i = 0; i < clientChannels.size(); i++)
			{
				std::map<std::string, Channel*>::iterator it = allChannels.find(clientChannels[i]->getName());
				if (it == allChannels.end())
					continue ;
				Channel *channel = it->second;
				channel->broadcastToChannel(QUIT((*client), killer, reasonToKill), client);
			}
			sendMSG(client->getFd(), QUIT((*client), killer, reasonToKill));
			_server.disconnectClient(*it->second);
			return ;
		}
	}
}

void	MsgHandler::handleDIE(Client &client)
{
	if (client.isIRCOp())
		_server.shutdown();
	else
		sendMSG(client.getFd(), ERR_NOPRIVILAGES(client));
}

void MsgHandler::respond(std::string &msg, Client &client)

{
	std::vector<std::string> msgData = split(msg, ' ');

	switch (getCommandType(msgData[0]))
	{
		case PASS: handlePASS(msgData[1], client);
			break ;
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
		case PRIVMSG: handlePRIVMSG(msg, client);
			break ;
		case UNKNOWN:
			break ;
		case KILL: handleKILL(msg, client);
			break ;
		case DIE: handleDIE(client);
			break ;
	}
}

void	MsgHandler::receiveMessage(Client &client)
{
	char		buffer[1024];
	
	ssize_t bytes_read = read(client.getFd(), buffer, sizeof(buffer) - 1);
	if (bytes_read <= 0)
	{
		_server.disconnectClient(client);
		return ;
	}
	buffer[bytes_read] = '\0';
	std::cout << buffer << RED << "(end)" << RESET << std::endl; // only for testing

	client.msgBuffer += buffer;
	size_t i;
	while ((i = client.msgBuffer.find("\r\n")) != std::string::npos)
	{
		std::string message = client.msgBuffer.substr(0, i);
		client.msgBuffer.erase(0, i + 2);

		std::vector<std::string> msgData = split(message, ' ');
		if (msgData[0] == "CAP" || message == "JOIN :")
			continue;
		else if (!client.isRegistered() && msgData[0] != "PASS")
		{
			if (msgData[0] == "NICK")
				sendMSG(client.getFd(), ERR_PASSWDMISMATCH(client));
			return ;
		}
		respond(message, client);
	}
}

void	MsgHandler::sendWelcomeProtocol(Client &client)
{
	sendMSG(client.getFd(), RPL_WELCOME(client));
	sendMSG(client.getFd(), RPL_YOURHOST(client));
	sendMSG(client.getFd(), RPL_CREATED(client));
	sendMSG(client.getFd(), RPL_MYINFO(client));
}

