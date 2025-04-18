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
		sendMSG(client.getFd(), ERR_NEEDMOREPARAMS(client, msgData[0]));
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

void MsgHandler::handlePRIVMSG(std::string &msg, Client &client)
{
	const std::vector<std::string> &trailing = split(msg, ':');
	const std::vector<std::string> &command = split(trailing[0], ' ');

	if (trailing.size() < 2 || command.size() < 2) {
		sendMSG(client.getFd(), ERR_NEEDMOREPARAMS(client, "PRIVMSG"));
		return warning("Insufficient parameters for PRIVMSG command");
	}
	std::string channelName = command.at(1);
    std::string message = trailing.at(1);

	if (msg.find("!quote") != std::string::npos)
	{
		_manager.forwardPrivateMessage(channelName, msg, client);
		handleQuote(channelName, client);
		return;
	}

	_manager.forwardPrivateMessage(channelName, message, client);
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
	std::vector<std::string> msgData = split(msg, ':');
	if (msgData.size() < 2) {
		sendMSG(client.getFd(), ERR_NEEDMOREPARAMS(client, msgData[0]));
		return warning("Insufficient parameters for JOIN command");
	}
	std::string message = (msgData[1].empty()) ? "No reason given" : msgData[1];

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


void	MsgHandler::handleQuote(const std::string& channelTarget, Client& client)
{
	if (!_server.getQuoteBot()->initiateConnection(_server))
	{
		info("Failed to connect to QuoteBot API");
		return;
	}
	_server.getQuoteBot()->setRequesterClient(&client);
	_server.getQuoteBot()->setRequesterChannel(channelTarget);
}

void MsgHandler::handlePART(std::vector<std::string> &msgData, Client &client)
{
	if (msgData.size() < 2) {
		sendMSG(client.getFd(), ERR_NEEDMOREPARAMS(client, msgData[0]));
		return warning("Insufficient parameters for MODE command");
	}
	_manager.removeFromChannel(msgData[1], client);
}

void MsgHandler::handlePASS(std::vector<std::string> &msgData, Client &client)
{
	if (msgData.size() < 2) {
		sendMSG(client.getFd(), ERR_NEEDMOREPARAMS(client, msgData[0]));
		return warning("Insufficient parameters for MODE command");
	}
	_server.validatePassword(msgData[1], client);
}

void MsgHandler::handleOPER(std::vector<std::string> &msgData, Client &client)
{
	if (msgData.size() < 3) {
		sendMSG(client.getFd(), ERR_NEEDMOREPARAMS(client, msgData[0]));
		return warning("Insufficient parameters for OPER command");
	}
	std::string &nickname = msgData[1];
	std::string &password = msgData[2];

	_server.validateIRCOp(nickname, password, client);
}

void MsgHandler::handleJOIN(std::vector<std::string> &msgData, Client &client)
{
	if (msgData.size() < 2) {
		sendMSG(client.getFd(), ERR_NEEDMOREPARAMS(client, msgData[0]));
		return warning("Insufficient parameters for JOIN command");
	}
	if (msgData[1][0] != '#') {
		sendMSG(client.getFd(), ERR_BADCHANMASK(client, msgData[1]));
		return warning("Channel " + msgData[1] + " does not exist");
	}
	std::string &channelName = msgData[1];
	std::string channelKey = (msgData.size() == 3) ? msgData[2] : "";

	_manager.addToChannel(channelName, channelKey, client);
}

void MsgHandler::handleINVITE(std::vector<std::string> &msgData, Client &client)
{
	if (msgData.size() < 3) {
		sendMSG(client.getFd(), ERR_NEEDMOREPARAMS(client, msgData[0]));
		return warning("Insufficient parameters for INVITE command");
	}
	std::string &channelName = msgData[1];
	std::string &nickname = msgData[2];
  	
	_manager.inviteClient(channelName, nickname, client);
}

void MsgHandler::handleKICK(std::string &msg, Client &client)
{
	std::string channelName, userToKick, reason;
	const std::vector<std::string> &msgData = split(msg, ':');
	const std::vector<std::string> &names = split(msgData[0], ' ');

	if (msgData.size() < 2 || names.size() < 3) {
		sendMSG(client.getFd(), ERR_NEEDMOREPARAMS(client, "KICK"));
		return warning("Insufficient parameters for KICK command");
	}
	channelName = names[1];
	userToKick = names[2];
	reason = (msgData.size() > 1) ? msgData[1] : "No reason given";

	_manager.kickFromChannel(channelName, userToKick, reason, client);
}
void MsgHandler::handleUSER(std::string &msg, Client &client)
{
	std::string username, hostname, IP, fullName;
	const std::vector<std::string> &msgData = split(msg, ':');
	const std::vector<std::string> &names = split(msgData[0], ' ');

	if (msgData.size() < 2 || names.size() < 4) {
		sendMSG(client.getFd(), ERR_NEEDMOREPARAMS(client, "KICK"));
		return warning("Insufficient parameters for KICK command");
	}

	client.assignUserData(username, hostname, IP, fullName);
}

void MsgHandler::respond(std::string &msg, Client &client)
{
	std::vector<std::string> msgData = split(msg, ' ');

	switch (getCommandType(msgData[0]))
	{
		case PASS: handlePASS(msgData, client);
			break ;
		case OPER: handleOPER(msgData, client); 
			break ;
		case JOIN: handleJOIN(msgData, client);
			break ;
		case PART: handlePART(msgData, client);
			break ;
		case INVITE: handleINVITE(msgData, client);
			break ;
		case KICK: handleKICK(msg, client);
			break ;
		case USER: handleUSER(msg, client);
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
		case PRIVMSG: handlePRIVMSG(msg, client);
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
