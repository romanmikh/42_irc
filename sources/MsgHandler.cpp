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
void	MsgHandler::assignUserData(std::string &msg, Client &client)

{
	std::vector<std::string> names = split(msg, ':');
	client.setFullName(names[1]);

	std::vector<std::string> moreNames = split(names[0], ' ');
	client.setUsername(moreNames[1]);
	client.setHostname(moreNames[2]);
	client.setIP(moreNames[3]);

	sendWelcomeProtocol(client);
}

void MsgHandler::handleMODE(std::string &msg, Client &client)
{
	std::vector<std::string> msgData = split(msg, ' ');

	if (msgData.size() < 2) {
		sendMSG(client.getFd(), ERR_NEEDMOREPARAMS(client));
		return warning("Insufficient parameters for MODE command");
	}

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
		return warning(client.nickname() + " is not an operator in channel " + channelName);
	}
}

void MsgHandler::handleTOPIC(std::string &msg, Client &client)
{
	std::vector<std::string> msgData = split(msg, ' ');
	Channel* chan = _manager.getChanByName(msgData[1]);

	if (!chan) {
		sendMSG(client.getFd(), ERR_NOSUCHCHANNEL(client, msgData[1]));
		return warning("Channel " + msgData[1] + " does not exist");
	}
	if (msgData.size() > 2)
	{
		if (chan->isTopicRestricted() && (chan->isClientChanOp(&client) || client.isIRCOp()))
		{
			chan->setTopic(msgData[2]);
			info(client.nickname() + " changed topic of channel " + chan->getName() + " to: " + chan->getTopic());
		}
		else if (!chan->isTopicRestricted())
		{
			chan->setTopic(msgData[2]);
			info(client.nickname() + " changed topic of channel " + chan->getName() + " to: " + chan->getTopic());
		}
		else
		{
			sendMSG(client.getFd(), ERR_CHANOPPROVSNEEDED(client, msgData[1]));
			warning(client.nickname() + " is not an operator in channel " + msgData[1]);
		}
	}
	else
		sendMSG(client.getFd(), RPL_TOPIC(client, chan->getName(), chan->getTopic()));
}

void	MsgHandler::validateIRCOp(std::string &nickname, std::string &password, Client &client)
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
		info(client.nickname() + " set as operator");
		client.setIRCOp(true);
		sendMSG(client.getFd(), RPL_YOUROPER(client));
	}
	else {
		sendMSG(client.getFd(), ERR_PASSWDMISMATCH(client));
	}
}

void	MsgHandler::validatePassword(std::string &password, Client &client)
{
	if (password == _server.getPassword())
	{
		client.setRegistered(true);
		sendMSG(client.getFd(), RPL_REGISTERED(client));
	}
	else
	{
		sendMSG(client.getFd(), ERR_PASSWDMISMATCH(client));
		_server.disconnectClient(client);
	}
}

void	MsgHandler::forwardPrivateMessage(std::string &msg, Client &client)
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
		sendMSG(client.getFd(), ERR_NOSUCHCHANNEL(client, channel));
        return warning("PRIVMSG channel is missing or invalid");
	}

	std::map<std::string, Channel*>::iterator it = allChannels.find(channel);
	if (it == allChannels.end())
	{
		sendMSG(client.getFd(), ERR_NOSUCHCHANNEL(client, channel));
		return warning("Channel " + channel + " does not exist in the server");
	}
	Channel* chan = it->second;
	if (chan->isEmpty())
		return warning("Channel is empty");

	chan->broadcastToChannel(STD_PREFIX(client) + " " + msg, &client);
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
			clientChannels[i]->broadcastToChannel(KILL(killer, victim, 
										clientChannels[i], reasonToKill), NULL);
			sendMSG(client->getFd(), RPL_NOTINCHANNEL((*client), 
												 clientChannels[i]->getName()));
		}
		sendMSG(client->getFd(), QUIT((*client), killer, reasonToKill));
		_server.disconnectClient(*client);
		return ;
	}
}

void MsgHandler::handleDIE(Client &client)
{
	if (!client.isIRCOp())
		return sendMSG(client.getFd(), ERR_NOPRIVILAGES(client));

	clients_t &allClients = _server.getClients();
	for (clients_t::iterator it = allClients.begin(); it != allClients.end(); ++it)
	{
		Client &c = *it->second;
		std::vector<Channel*> clientChannels = c.getClientChannels();
		for (size_t i = 0; i < clientChannels.size(); i++)
		{
			sendMSG(c.getFd(), RPL_NOTINCHANNEL(c, clientChannels[i]->getName()));
		}
		sendMSG(c.getFd(), DIE(c));
	}
	info("DIE command received. Server shutting down...");
	_server.shutdown();
}

void MsgHandler::handleNICK(std::string &nickname, Client &client)
{
	std::string newNickname = nickname;
	int i = 1;
	while (_server.getClientByNick(newNickname))
	{		
		std::ostringstream intTag;
		intTag << i++;
		newNickname = nickname + std::string(intTag.str());
	}
	client.setNickname(newNickname);
}

void MsgHandler::respond(std::string &msg, Client &client)
{
	std::vector<std::string> msgData = split(msg, ' ');

	switch (getCommandType(msgData[0]))
	{
		case PASS: validatePassword(msgData[1], client);
			break ;
		case USER: assignUserData(msg, client);
			break ;
		case NICK: if (msgData.size() == 2) handleNICK(msgData[1], client);
			break ;
		case JOIN: if (msgData.size() > 1 && msgData[1][0] == '#') _manager.addToChannel(client, msgData[1]);
			break ;
		case PART: if (msgData.size() > 1 && msgData[1][0] == '#') _manager.removeFromChannel(client, msgData[1]);
			break ;
		case INVITE: if (msgData.size() > 1 && msgData[2][0] == '#') _manager.inviteClient(msgData[1], msgData[2], client);
			break ;
		case KICK: if (msgData.size() > 1 && msgData[1][0] == '#') _manager.kickFromChannel(msg, client);
			break ;
		case MODE: if (msgData.size() > 1 && msgData[1][0] == '#') handleMODE(msg, client);
			break ;
		case TOPIC: handleTOPIC(msg, client);
			break ;
		case PING: sendMSG(client.getFd(), PONG);
			break ;
		case QUIT: _server.disconnectClient(client);
			break ;
		case OPER: if (msgData.size() == 3) validateIRCOp(msgData[1], msgData[2], client);
			break ;
		case PRIVMSG: forwardPrivateMessage(msg, client);
			break ;
		case KILL: handleKILL(msg, client);
			break ;
		case DIE: handleDIE(client);
			break ;
		case UNKNOWN:
			break ;
	}
}

bool	MsgHandler::badPassword(std::string &message, Client &client)
{
	std::vector<std::string> msgData = split(message, ' ');
	if (msgData[0] == "NICK")
	{
		sendMSG(client.getFd(), ERR_PASSWDMISMATCH(client));
		_server.disconnectClient(client);
		return (true);
	}
	return (false);
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
	std::cout << buffer; // for testing only 

	client.msgBuffer += buffer;
	size_t i;
	while ((i = client.msgBuffer.find("\r\n")) != std::string::npos)
	{
		std::string message = client.msgBuffer.substr(0, i);
		client.msgBuffer.erase(0, i + 2);
		if (!client.isRegistered() && badPassword(message, client))
			return ;
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

