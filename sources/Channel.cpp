#include "../include/Channel.hpp"

// ************************************************************************** //
//                       Constructors & Desctructors                          //
// ************************************************************************** //

Channel::Channel(std::string name) : _channelName(name), 
									 _channelPassword(""),
									 _channelTopic("No topic set"),
									 _channelTopicSetAt(""),
									 _channelTopicSetBy(""),
									 _channelIsInviteOnly(false),
									 _channelIsTopicRestricted(false),
									 _channelIsKeyProtected(false),
									 _channelIsLimitRestricted(false),
									 _channelClientCount(0),
									 _channelClientLimit(CHAN_CLIENT_LIMIT) {}

Channel::~Channel(void)
{
	info("Channel " + _channelName + " destroyed");
}


// ************************************************************************** //
//                               Accessors                                    //
// ************************************************************************** //

bool	Channel::isInviteOnly(void) const {	return _channelIsInviteOnly; }

bool	Channel::isTopicRestricted(void) const { return _channelIsTopicRestricted; }

bool	Channel::isKeyProtected(void) const { return _channelIsKeyProtected; }

bool	Channel::isLimitRestricted(void) const { return _channelIsLimitRestricted; }

std::string	Channel::getName(void) const { return _channelName; }

std::string	Channel::getPasskey(void) const { return _channelPassword; }

std::string Channel::getTopicSetAt(void) const { return _channelTopicSetAt; }

std::string	Channel::getTopicSetBy(void) const { return _channelTopicSetBy; }

std::string	Channel::getTopic(void) const { return _channelTopic; }

size_t	Channel::getClientCount(void) const { return _channelClientCount; }

size_t	Channel::getClientLimit(void) const { return _channelClientLimit; }

std::vector<Client *>&  Channel::getClients(void) { return _channelClients; }

std::vector<Client *>&  Channel::getOperators(void) { return _channelOperators; }

void	Channel::setName(std::string &name) { _channelName = name; }

void	Channel::setPassword(std::string &password) { _channelPassword = password; }

void	Channel::setTopic(std::string &topic, const std::string &nickname)
{ 
	_channelTopic = topic;
	_channelTopicSetAt = intToString(std::time(0));
	_channelTopicSetBy = nickname;
}

size_t	Channel::incClientCount(void) { return ++_channelClientCount; }

size_t	Channel::decClientCount(void)
{
	if (_channelClientCount > 0)
		return --_channelClientCount;
	else
	{
		error("Client count is already 0");
		return _channelClientCount;
	}
}


// ************************************************************************** //
//                             Public Functions                               //
// ************************************************************************** //

bool	Channel::isEmpty(void) const { return _channelClients.empty(); };

void	Channel::setModeI(std::string &mode, Client &client)
{
	_channelIsInviteOnly = (mode == "+i");
	broadcast(STD_PREFIX(client) + " MODE " + _channelName + " " + mode);
	if (mode == "+i")
		info("Channel " + _channelName + " is now invite only: " + boolToString(_channelIsInviteOnly));
	else if (mode == "-i")
		info("Channel " + _channelName + " is no longer invite only");
}

void	Channel::setModeT(std::string &mode, Client &client)
{
	if (mode == "+t")
	{
		_channelIsTopicRestricted = true;
		info("Channel " + _channelName + " is now topic restricted");
	}
	else if (mode == "-t")
	{
		_channelIsTopicRestricted = false;
		info("Channel " + _channelName + " is now topic unrestricted");
	}
	broadcast(STD_PREFIX(client) + " MODE " + _channelName + " " + mode);
}

void	Channel::setModeK(std::vector<std::string> &msgData, Client &client)
{
	std::string mode = msgData[2];

	if (mode[0] == '+' && (msgData.size() != 4 || msgData[3].empty())) {
		//sendMSG();
		return ;
	}
	if (mode[0] == '+') {
		_channelIsKeyProtected = true;
		_channelPassword = msgData[3];
	}
	else {
		_channelIsKeyProtected = false;
		_channelPassword = "";
	}
	info("Channel " + _channelName + " is now key protected: " + boolToString(_channelIsKeyProtected));
	broadcast(STD_PREFIX(client) + " MODE " + _channelName + " " + mode);
}

void	Channel::setModeO(std::vector<std::string> &msgData, Client &client, Server &server)
{
	std::string mode = msgData[2];

	if (msgData.size() != 4) {
		//sendMSG();
		return ;
	}
	Client *recipient = server.getClientByNick(msgData[3]);
	if (!recipient) {
		warning("Client with nickname " + msgData[3] + " not found");
		return sendMSG(client.getFd(), ERR_NOSUCHNICK(client, msgData[3]));
	}
	if (mode[0] == '+') {
		addChanOp(recipient);	
		info("Client " + recipient->nickname() + "added as " + _channelName + " channel Operator by " + client.nickname());
	}
	else {
		removeChanOp(recipient);	
		info("Client " + recipient->nickname() + "removed as " + _channelName + " channel Operator by " + client.nickname());
	}
	broadcast(STD_PREFIX(client) + " MODE " + _channelName + " " + mode);
}

void	Channel::setModeL(std::vector<std::string> &msgData, Client &client)
{
	std::string mode = msgData[2];

	if (mode[0] == '+' && (msgData.size() != 4 || msgData[3].empty()))
	{
		//sendMSG(client.getFd(), ERR_BADCHANNELKEY(client, _channelName));
		return ;
	}
	if (mode[0] == '+')
	{
		_channelIsLimitRestricted = true;
		std::stringstream size(msgData[3]);
		size >> _channelClientLimit;
	}
	else if(mode[0] == '-') 
	{
		_channelIsLimitRestricted = false;
		_channelClientLimit = 0;
	}
	info("Channel " + _channelName + " is now limit restricted (currently " + sizeToString(_channelClientCount) + "/" +sizeToString(_channelClientLimit) + "): " + boolToString(_channelIsLimitRestricted));
	broadcast(STD_PREFIX(client) + " MODE " + _channelName + " " + mode);
}

bool	Channel::hasClient(Client* client) const
{
	for (std::vector<Client *>::const_iterator it = _channelClients.begin(); \
										   it != _channelClients.end(); ++it) {
		if (*it == client)
			return true;
	}
	return false;
}

bool    Channel::isClientChanOp(Client* client) const
{
	for (std::vector<Client *>::const_iterator it = _channelOperators.begin(); \
										it != _channelOperators.end(); ++it)
	{
		if (*it == client)
			return true;
	}
	return false;
}

void	Channel::addChanOp(Client* client)
{
	// if (client->isIRCOp())
	// 	return warning(client->nickname() + " is a global operator");
	if (isClientChanOp(client))
		return warning(client->nickname() + " is already an operator in channel " + _channelName);
	_channelOperators.push_back(client);
	broadcast(STD_PREFIX((*client)) + " MODE " + _channelName + " +o ");
	info(client->nickname() + " is now an operator in channel " + _channelName);
}

void	Channel::removeChanOp(Client* client)
{
	if (client->isIRCOp())
		return warning(client->nickname() + " is a global operator");
	if (!isClientChanOp(client))
		return warning(client->nickname() + " is not an operator in channel " + _channelName);
	for (std::vector<Client *>::iterator it = _channelOperators.begin(); it != _channelOperators.end(); ++it) {
		if (*it == client) {
			_channelOperators.erase(it);
			return;
		}
	}
	broadcast(STD_PREFIX((*client)) + " MODE " + _channelName + " -o ");
	info(client->nickname() + " is no longer an operator in channel " + _channelName);
}

void	Channel::broadcast(std::string message)
{
	if (isEmpty())
		return warning("Channel is empty");
	if (message.empty())
		return warning("Empty message");
	if (message.length() > 512)
		return warning("Message too long");
	for (std::vector<Client *>::const_iterator it = _channelClients.begin(); it != _channelClients.end(); ++it) {
		sendMSG((*it)->getFd(), (message + "\r\n"));
	}
}

void	Channel::broadcastSilent(std::string message, Client *client)
{
	if (isEmpty())
		return warning("Channel is empty");
	if (message.empty())
		return warning("Empty message");
	if (message.length() > 512)
		return warning("Message too long");
	for (std::vector<Client *>::const_iterator it = _channelClients.begin(); it != _channelClients.end(); ++it) {
		if (*it != client)
			sendMSG((*it)->getFd(), (message + "\r\n"));
	}
}