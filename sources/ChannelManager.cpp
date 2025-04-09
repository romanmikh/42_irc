#include "../include/ChannelManager.hpp"

// ************************************************************************** //
//                       Constructors & Desctructors                          //
// ************************************************************************** //

ChannelManager::ChannelManager(Server &server) : _server(server) {}

ChannelManager::~ChannelManager(void)
{
	for (channels_t::iterator it = _channels.begin(); it != _channels.end(); ++it) {
		delete it->second;
	}
	_channels.clear();
}


// ************************************************************************** //
//                               Accessors                                    //
// ************************************************************************** //

const ChannelManager::channels_t&	ChannelManager::getChannels(void) const { return _channels; }

size_t	ChannelManager::getChannelCount(void) const { return _channelCount; }

size_t	ChannelManager::incChannelCount(void) { return ++_channelCount; }

size_t	ChannelManager::decChannelCount(void)
{
	if (_channelCount > 0)
		return --_channelCount;
	else {
		error("Channel count is already 0");
		return _channelCount;
	}
}


// ************************************************************************** //
//                             Public Functions                               //
// ************************************************************************** //

bool	ChannelManager::channelExists(const std::string& channelName) const { return _channels.find(channelName) != _channels.end(); }

Channel*	ChannelManager::getChanByName(const std::string& channelName)
{
	channels_t::iterator it = _channels.find(channelName);
	if (it != _channels.end())
		return it->second;
	return NULL;
}

Channel	*ChannelManager::createChannel(const std::string &channelName)
{
    Channel *newChannel = new Channel(channelName);
	_channels.insert(channel_pair_t (channelName, newChannel));
	info("Channel created: " + channelName);
	incChannelCount();
	return (newChannel);
}

void	ChannelManager::deleteChannel(const std::string &channelName)
{
	channels_t::iterator it = _channels.find(channelName);
	if (it != _channels.end())
	{
		delete it->second;
		_channels.erase(it);
		info("Channel deleted: " + channelName);
		decChannelCount();
	}
	else
		warning("Channel " + channelName + " does not exist");
}

bool ChannelManager::chanPermissionsFail(Client& client, const std::string& channelName, std::string &channelKey)
{
	Channel* channel = _channels[channelName];

	if (channel->isLimitRestricted() && (channel->getClientCount() >= channel->getClientLimit())) 
	{
		sendMSG(client.getFd(), ERR_CHANNELISFULL(client, channelName));
		return (true);
	}
	if (channel->isKeyProtected() && channelKey != channel->getPassword()) {
		sendMSG(client.getFd(), ERR_BADCHANNELKEY(client, channelName));
		return (true);
	}
	if (channel->isInviteOnly() && !(client.isInvited(channelName) || channel->isClientChanOp(&client) || client.isIRCOp()))
	{
		sendMSG(client.getFd(), ERR_INVITEONLYCHAN(client, channelName));
		return (true);
	}
	return (false);
}

void	ChannelManager::addToChannel(std::vector<std::string> &msgData, Client &client)
{
	std::string &channelName = msgData[1];
	Channel	*channel = getChanByName(channelName);

	if (!channel) {
		channel = createChannel(channelName);
		channel->addChanOp(&client);
	}
	std::string channelKey = (msgData.size() == 3) ? msgData[2] : "";
	if (chanPermissionsFail(client, channelName, channelKey)) {
		return ;
	}
	std::vector<Client *>& clients = channel->getClients();
	if (std::find(clients.begin(), clients.end(), &client) == clients.end()) {
		clients.push_back(&client);
		client.joinChannel(*this, channelName);
		client.delClientChannelInvite(channelName);
		channel->incClientCount();
	}
	info(client.username() + " joined channel " + channelName);
	// sendMSG(client.getFd(), RPL_TOPIC(client, channel->getName(), channel->getTopic()));
}

void ChannelManager::removeFromChannel(const std::string& channelName, Client& client)
{
    Channel* channel = getChanByName(channelName);

	if (!channel)
	{
		sendMSG(client.getFd(), ERR_NOSUCHCHANNEL(client, channelName));
		return warning("Channel " + channelName + " does not exist");
    }
    std::vector<Client*>& channelClients = channel->getClients();
    std::vector<Client*>::iterator clientIt = std::find(channelClients.begin(), channelClients.end(), &client);
    if (clientIt != channelClients.end()) 
    {
        std::string PARTmsg = STD_PREFIX(client) + " PART " + channelName + " :bye!" + "\r\n";
        sendMSG(client.getFd(), PARTmsg);
        
        if (channel->isClientChanOp(&client))
            channel->removeChanOp(&client);
        
        client.leaveChannel(*this, channelName);
        channelClients.erase(clientIt);
        channel->decClientCount();
            
        info(client.username() + " removed from channel " + channelName);
        sendMSG(client.getFd(), RPL_NOTINCHANNEL(client, channelName));
    }
    if (channel->getClients().empty())
        deleteChannel(channelName);
}

void	ChannelManager::kickFromChannel(std::string &msg, Client &kicker)
{
	std::string channelName, userToKick, reason;
	const std::vector<std::string> &msgData = split(msg, ':');
	reason = (msgData.size() > 1) ? msgData[1] : "No reason given";
	
	const std::vector<std::string> &names = split(msgData[0], ' ');
	channelName = names[1];
	userToKick = names[2];

	Channel* chan = getChanByName(channelName);
	if (!chan)
	{
		sendMSG(kicker.getFd(), ERR_NOSUCHCHANNEL(kicker, channelName));
		return warning("Channel " + channelName + " does not exist");
	}
	if (chan->isClientChanOp(&kicker) || kicker.isIRCOp())
	{
		info(kicker.nickname() + " kicked " + userToKick + " from channel " + channelName);
		Client *client = _server.getClientByNick(userToKick);
		if (client)
		{
			chan->broadcastToChannel(KICK(kicker, channelName, client->nickname(), reason), &kicker);
			removeFromChannel(channelName, *client);
			sendMSG(client->getFd(), KICK(kicker, channelName, client->nickname(), reason));
		}
	}
	else 
	{
		sendMSG(kicker.getFd(), ERR_CHANOPPROVSNEEDED(kicker, channelName));
		warning(kicker.nickname() + " is not an operator in channel " + channelName);
	}
}

void	ChannelManager::inviteClient(std::string &nickname, const std::string& channelName, Client &client)
{
  	Channel* chan = getChanByName(channelName);
  	
	if (!chan)
	{
		sendMSG(client.getFd(), ERR_NOSUCHCHANNEL(client, channelName));
		return warning("Channel " + channelName + " does not exist");
	}
	if (chan->isClientChanOp(&client) || client.isIRCOp())
	{
		Client* targetClient = _server.getClientByNick(nickname);
		if (!targetClient)
			return warning("Client " + nickname + " not found");
		sendMSG(targetClient->getFd(), INVITE(client, nickname, channelName));
		sendMSG(client.getFd(), RPL_INVITING(client, nickname, channelName));
		targetClient->addClientChannelInvite(channelName);
		info(client.username() + " invited " + nickname + " to channel " + channelName);
	}
	else
	{
		sendMSG(client.getFd(), ERR_CHANOPPROVSNEEDED(client, channelName));
		return warning(client.nickname() + " is not an operator in channel " + channelName);
	}
}

void	ChannelManager::setChanMode(std::vector<std::string> &msgData, Client &client)
{
	if (msgData.size() < 3 || !strchr("+-", msgData[2][0]) || !strchr("itkol", msgData[2][1]))
	{
		sendMSG(client.getFd(), ERR_UNKNOWNMODE(client, msgData[2]));
		return warning("Invalid mode: " + msgData[1] + ". +/- {i, t, k, o, l}");
	}
	std::string channelName = msgData[1];
	std::string mode = msgData[2];

	Channel *channel = getChanByName(channelName);
	if (mode[1] == 'i')
		channel->setModeI(mode, client);
	else if (mode[1] == 't')
		channel->setModeT(mode, client);
	else if (mode[1] == 'k')
		channel->setModeK(msgData, client);
	else if (mode[1] == 'o')
		channel->setModeO(msgData, client, _server);
	else if (mode[1] == 'l')
		channel->setModeL(msgData, client);
}