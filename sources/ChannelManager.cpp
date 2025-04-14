#include "../include/ChannelManager.hpp"

// ************************************************************************** //
//                       Constructors & Desctructors                          //
// ************************************************************************** //

ChannelManager::ChannelManager(Server &server) : _server(server), _channelCount(0) {}

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

bool ChannelManager::chanRestrictionsFail(Client& client, const std::string& channelName, std::string &channelKey)
{
	Channel* channel = _channels[channelName];

	if (channel->isLimitRestricted() && (channel->getClientCount() >= channel->getClientLimit())) 
	{
		sendMSG(client.getFd(), ERR_CHANNELISFULL(client, channelName));
		return (true);
	}
	if (channel->isKeyProtected() && channelKey != channel->getPasskey()) {
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

void	ChannelManager::addToChannel(std::string &channelName, std::string &channelKey, Client &client)
{
	Channel	*channel = getChanByName(channelName);

	if (!channel) {
		channel = createChannel(channelName);
		channel->addChanOp(&client);
	}
	if (chanRestrictionsFail(client, channelName, channelKey) || channel->hasClient(&client)) {
		return ;
	}
	channel->getClients().push_back(&client);
	channel->incClientCount();
	client.getClientChannels().push_back(channel);
	client.delChannelInvite(channelName);
	info(client.nickname() + " joined channel " + channelName);
	sendMSG(client.getFd(), RPL_TOPIC(client, channel->getName(), channel->getTopic()));
	if (channel->getTopic() != "No topic set") {
		sendMSG(client.getFd(), RPL_TOPICWHOTIME(client, channel->getName(), channel->getTopicSetBy(), channel->getTopicSetAt()));
	}
}

void ChannelManager::removeFromChannel(const std::string& channelName, Client& client)
{
    Channel* channel = getChanByName(channelName);

	if (!channel) {
		sendMSG(client.getFd(), ERR_NOSUCHCHANNEL(client, channelName));
		return warning("Channel " + channelName + " does not exist");
    }
    std::vector<Client*>& channelClients = channel->getClients();
    std::vector<Client*>::iterator clientIt = std::find(channelClients.begin(), channelClients.end(), &client);
    if (clientIt == channelClients.end()) {
		sendMSG(client.getFd(), ERR_NOTONCHANNEL(client, channelName));
		return warning(client.nickname() + " is not an operator in channel " + channelName);
	}

    channelClients.erase(clientIt);
    channel->decClientCount();
    channel->broadcast(PART(client, channelName)); 
    sendMSG(client.getFd(), RPL_NOTINCHANNEL(client, channelName));
    info(client.nickname() + " removed from channel " + channelName);
    client.popClientChannel(*this, channelName);

    if (channel->isClientChanOp(&client)) {
        channel->removeChanOp(&client);
	}
    if (channel->getClients().empty()) {
        deleteChannel(channelName);
	}
	else if (channel->getOperators().empty()) {
		channel->addChanOp(channel->getClients().front());
	}
}

void	ChannelManager::kickFromChannel(std::string &channelName, std::string &userToKick, std::string &reason, Client &kicker)
{
	Channel* chan = getChanByName(channelName);
	if (!chan)
	{
		sendMSG(kicker.getFd(), ERR_NOSUCHCHANNEL(kicker, channelName));
		return warning("Channel " + channelName + " does not exist");
	}
	if (!chan->isClientChanOp(&kicker) && !kicker.isIRCOp())
	{
		sendMSG(kicker.getFd(), ERR_CHANOPPROVSNEEDED(kicker, channelName));
		return warning(kicker.nickname() + " is not an operator in channel " + channelName);
	}
	Client *client = _server.getClientByNick(userToKick);
	if (client)
	{
		chan->broadcast(KICK(kicker, channelName, client->nickname(), reason));
		removeFromChannel(channelName, *client);
	}
	info(kicker.nickname() + " kicked " + userToKick + " from channel " + channelName);
}

void	ChannelManager::inviteClient(std::string &channelName, std::string &nickname, Client &client)
{
  	Channel* chan = getChanByName(channelName);
	if (!chan)
	{
		sendMSG(client.getFd(), ERR_NOSUCHCHANNEL(client, channelName));
		return warning("Channel " + channelName + " does not exist");
	}
	if (!chan->isClientChanOp(&client) && !client.isIRCOp())
	{
		sendMSG(client.getFd(), ERR_CHANOPPROVSNEEDED(client, channelName));
		return warning(client.nickname() + " is not an operator in channel " + channelName);
	}
	Client* targetClient = _server.getClientByNick(nickname);
	if (!targetClient)
	{
		sendMSG(client.getFd(), ERR_NOSUCHNICK(client, channelName));
		return warning("Client " + nickname + " not found");
	}
	sendMSG(targetClient->getFd(), INVITE(client, nickname, channelName));
	sendMSG(client.getFd(), RPL_INVITING(client, nickname, channelName));
	targetClient->addChannelInvite(channelName);
	info(client.nickname() + " invited " + nickname + " to channel " + channelName);
}

void	ChannelManager::forwardPrivateMessage(std::string &channelName, std::string &message, Client &client)
{
	std::map<std::string, Channel*> allChannels = getChannels();

	std::map<std::string, Channel*>::iterator it = allChannels.find(channelName);
	if (it == allChannels.end())
	{
		sendMSG(client.getFd(), ERR_NOSUCHCHANNEL(client, channelName));
        return warning("PRIVMSG channel is missing or invalid");
	}
	Channel* channel = it->second;
	if (!channel->hasClient(&client) && !client.isBot()) {
		sendMSG(client.getFd(), ERR_NOTONCHANNEL(client, channelName));
		warning("client" + client.nickname() + " not in channel " + channelName);
	}
	if (channel->isEmpty())
		return warning("Channel is empty");
	channel->broadcastSilent(STD_PREFIX(client) + " " + message, &client);
}

void	ChannelManager::setChanMode(std::vector<std::string> &msgData, Client &client)
{
	if (msgData.size() < 3 || msgData[2].length() != 2 || !strchr("+-", msgData[2][0]) || !strchr("itkol", msgData[2][1]))
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