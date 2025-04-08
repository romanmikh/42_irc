#include "../include/ChannelManager.hpp"

// ************************************************************************** //
//                       Constructors & Desctructors                          //
// ************************************************************************** //
ChannelManager::ChannelManager(Server &server) : _server(server) {}

ChannelManager::~ChannelManager(void) {
	for (channels_t::iterator it = _channels.begin(); it != _channels.end(); ++it) {
		delete it->second;
	}
	_channels.clear();
}

// ************************************************************************** //
//                               Accessors                                    //
// ************************************************************************** //
const ChannelManager::channels_t&   ChannelManager::getChannels(void) const {
	return _channels;
}

size_t                              ChannelManager::getChannelCount(void) const {
	return _channelCount;
}

size_t                              ChannelManager::incChannelCount(void) {
	return ++_channelCount;
}

size_t                              ChannelManager::decChannelCount(void) {
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
bool    ChannelManager::channelExists(const std::string& channelName) const {
	return _channels.find(channelName) != _channels.end();
}

Channel* ChannelManager::getChanByName(const std::string& channelName) {
	channels_t::iterator it = _channels.find(channelName);
	if (it != _channels.end()) {
		return it->second;
	}
	return NULL;
}

void    ChannelManager::createChannel(const std::string &channelName, Client *firstClient)
{
    Channel *newChannel = new Channel(channelName);
	_channels.insert(channel_pair_t (channelName, newChannel));
    newChannel->addChanOp(firstClient);
	info("Channel created: " + channelName);
	incChannelCount();
}

void    ChannelManager::deleteChannel(const std::string &channelName) {
	channels_t::iterator it = _channels.find(channelName);
	if (it != _channels.end()) {
		delete it->second;
		_channels.erase(it);
		info("Channel deleted: " + channelName);
		decChannelCount();
	}
	else {
		warning("Channel " + channelName + " does not exist");
	}
}

void ChannelManager::addToChannel(Client& client, const std::string& channelName) {
	if (_channels.find(channelName) == _channels.end()) {
		createChannel(channelName, &client);
	}
	Channel* channel = _channels[channelName];
	if (channel->isInviteOnly())
	{
		if (!channel->isClientChanOp(&client) && !client.isIRCOp()) {
			sendMSG(client.getFd(), ERR_INVITEONLYCHAN(client, channelName));
			return;
		}
	}
	else if (channel->isLimitRestricted() && (channel->getClientCount() >= channel->getClientLimit())) 
	{
		sendMSG(client.getFd(), ERR_CHANNELISFULL(client, channelName));
		return;
	}
	std::vector<Client *>& clients = channel->getClients();
	if (std::find(clients.begin(), clients.end(), &client) == clients.end()) {
		clients.push_back(&client);
		client.joinChannel(*this, channelName);
	}
	info(client.username() + " joined channel " + channelName);
	// sendMSG(client.getFd(), RPL_TOPIC(client, channel->getName(), channel->getTopic()));
	channel->incClientCount();
}

void ChannelManager::removeFromChannel(Client& client, const std::string& channelName)
{
    Channel* channel = getChanByName(channelName);

	if (!channel) {
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
    if (channel->getClients().empty()) {
        deleteChannel(channelName);
    }
}
void ChannelManager::kickFromChannel(std::string &msg, Client &kicker)
{
	std::string channelName, userToKick, reason;
	const std::vector<std::string> &msgData = split(msg, ':');
	const std::vector<std::string> &names = split(msgData[0], ' ');
	
	reason = (msgData.size() > 1) ? msgData[1] : "No reason given";
	channelName = names[1];
	userToKick = names[2];

	Channel* chan = getChanByName(channelName);
	if (!chan) {
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
			removeFromChannel(*client, channelName);
			sendMSG(client->getFd(), KICK(kicker, channelName, client->nickname(), reason));
		}
	}
	else 
	{
		sendMSG(kicker.getFd(), ERR_CHANOPPROVSNEEDED(kicker, channelName));
		warning(kicker.nickname() + " is not an operator in channel " + channelName);
	}
}
void ChannelManager::inviteClient(std::string &nickname, const std::string& channelName, Client &client)
{
  	Channel* chan = getChanByName(channelName);
  	
	if (!chan) {
		sendMSG(client.getFd(), ERR_NOSUCHCHANNEL(client, channelName));
		return warning("Channel " + channelName + " does not exist");
	}
	if (chan->isClientChanOp(&client) || client.isIRCOp())
	{
		Client* targetClient = _server.getClientByNick(nickname);
        if (!targetClient) {
            return warning("Client " + nickname + " not found");
        }
	    sendMSG(targetClient->getFd(), INVITE(client, nickname, channelName));
	    sendMSG(client.getFd(), RPL_INVITING(client, nickname, channelName));
		sendMSG(targetClient->getFd(), JOIN(client, nickname, channelName));
		error(JOIN(client, nickname, channelName));
		info(client.username() + " invited " + nickname + " to channel " + channelName);
	    addToChannel(*targetClient, channelName);
	}
	else {
		sendMSG(client.getFd(), ERR_CHANOPPROVSNEEDED(client, channelName));
		return warning(client.nickname() + " is not an operator in channel " + channelName);
	}
}
