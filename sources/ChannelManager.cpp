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

void    ChannelManager::createChannel(std::string channelName) {
	_channels.insert(channel_pair_t (channelName, new Channel(channelName)));
	info("Channel created: " + channelName);
	incChannelCount();
}

void    ChannelManager::deleteChannel(std::string channelName) {
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
		createChannel(channelName);
	}
	Channel* channel = _channels[channelName];
	if (channel->isLimitRestricted() && (channel->getClientCount() >= channel->getClientLimit())) 
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
	channel->incClientCount();
}

void ChannelManager::removeFromChannel(Client& client, const std::string& channelName)
{
	Channel* channel = getChanByName(channelName);
  // we need to send an ERR 403 NOSUCHCHANNEL here
	if (!channel)
		return warning("Channel " + channelName + " does not exist");

	std::vector<Client*>& channelClients = channel->getClients();
	std::vector<Client*>::iterator clientIt = std::find(channelClients.begin(), channelClients.end(), &client);
	if (clientIt != channelClients.end()) 
	{
		std::string PARTmsg = CMD_STD_FMT(client) + " PART " + channelName + " :bye!" + "\r\n";
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

void ChannelManager::inviteClient(std::string username, const std::string& channel, Client client)
{
	if (client.isIRCOp())
	{
		Client* targetClient = _server.getClientByUser(username);
		if (!targetClient)
			return warning("Client " + username + " not found");
		addToChannel(*targetClient, channel);
		return info(client.username() + " invited " + username + " to channel " + channel);
	}
	else if (!client.isIRCOp() && !channelExists(channel))
		return warning(client.username() + " is not an IRC operator so cannot " + 
					"create channel " + channel + "and invite " + username);
	else if (channelExists(channel))
	{
		Channel* chan = getChannels().at(channel);
		if (chan->isClientChanOp(&client))
		{
			addToChannel(client, channel);
			return info(client.username() + " invited " + username + " to channel " + channel);
		}
		else {
			return warning(client.username() + " is not an operator in channel " + channel);
		}
	}
}

