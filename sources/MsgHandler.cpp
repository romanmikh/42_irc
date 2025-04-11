#include "../include/MsgHandler.hpp"
#include "../include/irc.hpp"

// ************************************************************************** //
//                       Constructors & Desctructors                          //
// ************************************************************************** //
MsgHandler::MsgHandler(Server &server, ChannelManager &manager)
	: _server(server), _manager(manager) {};

MsgHandler::~MsgHandler() {};

// ************************************************************************** //
//                             Public Functions                               //
// ************************************************************************** //


void MsgHandler::handleMODE(std::vector<std::string> &msgData, Client &client)
{
	if (msgData.size() < 2) {
		sendMSG(client.getFd(), ERR_NEEDMOREPARAMS(client));
		return warning("Insufficient parameters for MODE command");
	}
	// silently ignore user modes
	if (_server.getClientByNick(msgData[1]))
		return ;

	std::string channelName = msgData[1];
	Channel* chan = _manager.getChanByName(channelName);
	if (!chan) {
		sendMSG(client.getFd(), ERR_NOSUCHCHANNEL(client, channelName));
		return warning("Channel " + channelName + " does not exist");
	}
	if (chan->isClientChanOp(&client) || client.isIRCOp())
		_manager.setChanMode(msgData, client);
	else
	{
		sendMSG(client.getFd(), ERR_CHANOPPROVSNEEDED(client, channelName));
		sendMSG(client.getFd(), SERVER_NAME + " " + client.nickname() + " NOTICE :You are not a channel operator\r\n");
		return warning(client.nickname() + " is not an operator in channel " + channelName);
	}
}

void MsgHandler::handleTOPIC(std::string &msg, Client &client)
{
	std::string topic = split(msg, ':').back();
	std::string channelName = split(msg, ' ').at(1);
	Channel* channel = _manager.getChanByName(channelName);

	if (!channel) {
		sendMSG(client.getFd(), ERR_NOSUCHCHANNEL(client, channelName));
		return warning("Channel " + channelName + " does not exist");
	}
	if (!channel->hasClient(&client))
	{
		sendMSG(client.getFd(), ERR_NOTONCHANNEL(client, channelName));
		return warning(client.nickname() + " is not an operator in channel " + channelName);
	}
	if (channel->isTopicRestricted() && !(channel->isClientChanOp(&client) || client.isIRCOp()))
	{
		sendMSG(client.getFd(), ERR_CHANOPPROVSNEEDED(client, channelName));
		return warning(client.nickname() + " is not an operator in channel " + channelName);
	}
	channel->setTopic(topic, client.nickname());
	info(client.nickname() + " changed topic of channel " + channel->getName() + " to: " + topic);
	channel->broadcast(RPL_TOPIC(client, channel->getName(), topic));
}

void	MsgHandler::forwardPrivateMessage(std::string &msg, Client &client)
{
	std::map<std::string, Channel*> allChannels = _manager.getChannels();
    std::string message = split(msg, ':').back();
	std::string channelName = split(msg, ' ').at(1);

	std::map<std::string, Channel*>::iterator it = allChannels.find(channelName);
	if (it == allChannels.end())
	{
		sendMSG(client.getFd(), ERR_NOSUCHCHANNEL(client, channelName));
        return warning("PRIVMSG channel is missing or invalid");
	}
	Channel* channel = it->second;
	if (channel->isEmpty())
		return warning("Channel is empty");
	channel->broadcastSilent(STD_PREFIX(client) + " " + msg, &client);
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

	Client *client = _server.getClientByNick(userToKill);
	if (client)
	{
		Client& victim = *client;

		std::vector<Channel*> clientChannels = client->getClientChannels();
		for (size_t i = 0; i < clientChannels.size(); i++)
		{
			clientChannels[i]->broadcast(KILL(killer, victim, clientChannels[i], reasonToKill));
			_manager.removeFromChannel(clientChannels[i]->getName(), victim);
		}
		sendMSG(victim.getFd(), QUITKILLEDBY(victim, killer, reasonToKill));
		_server.disconnectClient(&victim);
		return ;
	}
}

void MsgHandler::handleQUIT(std::string &msg, Client &client)
{
	std::string message = split(msg, ':').back();
	if (message.empty())
		message = "No reason given";

	std::vector<Channel*>& clientChannels = client.getClientChannels();
	for (size_t i = 0; i < clientChannels.size(); i++)
	{
		clientChannels[i]->broadcast(QUIT(client, message));
		_manager.removeFromChannel(clientChannels[i]->getName(), client);
	}
	_server.disconnectClient(&client);
}

void MsgHandler::handleDIE(Client &client)
{
	if (!client.isIRCOp())
		return sendMSG(client.getFd(), ERR_NOPRIVILAGES(client));

	clients_t &allClients = _server.getClients();
	for (clients_t::iterator it = allClients.begin(); it != allClients.end(); ++it)
	{
		Client &c = *it->second;
		std::vector<Channel *> clientChannels = c.getClientChannels();
		for (size_t i = 0; i < clientChannels.size();i++)
		{
  		  	sendMSG(c.getFd(), RPL_NOTINCHANNEL(c, clientChannels[i]->getName()));
		}
		sendMSG(c.getFd(), DIE(c));
	}
	info("DIE command received. Server shutting down...");
	_server.shutdown();
}

void MsgHandler::handleNICK(std::vector<std::string> &msgData, Client &client)
{
	if (msgData.size() < 2) {
		return sendMSG(client.getFd(), ERR_NONICKNAMEGIVEN(client));
	}

	std::string nickname = msgData[1];
	if (client.nickname() == "undefined")
	{
		int i = 1;
		while (_server.getClientByNick(nickname))
		{		
			nickname = msgData[1] + intToString(i++);
		}
	}
	else if ((_server.getClientByNick(nickname))) {
		return sendMSG(client.getFd(), ERR_NICKNAMEINUSE(client, msgData[1]));
	}
	client.setNickname(nickname);
}

void MsgHandler::respond(std::string &msg, Client &client)
{
	std::vector<std::string> msgData = split(msg, ' ');

	switch (getCommandType(msgData[0]))
	{
		case PASS: _server.validatePassword(msgData[1], client);
			break ;
		case OPER: _server.validateIRCOp(msgData, client);
			break ;
		case JOIN: _manager.addToChannel(msgData, client);
			break ;
		case PART: _manager.removeFromChannel(msgData[1], client);
			break ;
		case INVITE: _manager.inviteClient(msgData[1], msgData[2], client);
			break ;
		case KICK: _manager.kickFromChannel(msg, client);
			break ;
		case USER: client.assignUserData(msg);
			break ;
		case QUIT: handleQUIT(msg, client);
			break ;
		case NICK: handleNICK(msgData, client);
			break ;
		case MODE: handleMODE(msgData, client);
			break ;
		case TOPIC: handleTOPIC(msg, client);
			break ;
		case KILL: handleKILL(msg, client);
			break ;
		case DIE: handleDIE(client);
			break ;
		case PING: sendMSG(client.getFd(), PONG);
			break ;
		case PRIVMSG: forwardPrivateMessage(msg, client);
			break ;
		case UNKNOWN:
			break ;
	}
}

void	MsgHandler::receiveMessage(Client &client)
{
	char		buffer[1024];
	
	ssize_t bytes_read = read(client.getFd(), buffer, sizeof(buffer) - 1);
	if (bytes_read <= 0) {
		return _server.disconnectClient(&client);
	}
	buffer[bytes_read] = '\0';
	if (!strcmp(buffer, "\r\n")) {
		return ;
	}
	std::cout << buffer; // for testing only 

	client.msgBuffer += buffer;
	size_t i;
	while ((i = client.msgBuffer.find("\r\n")) != std::string::npos)
	{
		std::string message = client.msgBuffer.substr(0, i);
		client.msgBuffer.erase(0, i + 2);
		if (!client.isRegistered() && split(message, ' ').front() == "NICK")
		{
			error("Invalid or no password: client disconnected.");
			sendMSG(client.getFd(), ERR_PASSWDMISMATCH(client));
			_server.disconnectClient(&client);
			return ;
		}
		respond(message, client);  // maybe take PASS out of this funciton
	}
}
