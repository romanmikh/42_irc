#include "../include/ChannelManager.hpp"

// ************************************************************************** //
//                       Constructors & Desctructors                          //
// ************************************************************************** //
ChannelManager::ChannelManager(void) {}

ChannelManager::~ChannelManager(void){
    for (channels_t::iterator it = _channels.begin(); it != _channels.end(); ++it) {
        delete it->second;
    }
    _channels.clear();
}

// ************************************************************************** //
//                               Accessors                                    //
// ************************************************************************** //
const ChannelManager::channels_t&      ChannelManager::getChannels(void) const {
    return _channels;
}

// ************************************************************************** //
//                             Public Functions                               //
// ************************************************************************** //
void    ChannelManager::createChannel(std::string channelName) {
    _channels.insert(channel_pair_t (channelName, new Channel(channelName)));
    info("Channel created: " + channelName);
}

void    ChannelManager::deleteChannel(std::string channelName) {
    channels_t::iterator it = _channels.find(channelName);
    if (it != _channels.end()) {
        delete it->second;
        _channels.erase(it);
        info("Channel deleted: " + channelName);
    }
    else {
        warning("Channel " + channelName + " does not exist");
        throw ChannelManager::ChannelNonExistentException();
    }
}

void ChannelManager::joinChannel(Client& client, const std::string& channelName) {
    if (_channels.find(channelName) == _channels.end()) {
        createChannel(channelName);
    }
    Channel* channel = _channels[channelName];
    std::vector<Client *>& clients = channel->getClients();
    if (std::find(clients.begin(), clients.end(), &client) == clients.end()) {
        clients.push_back(&client);
        client.joinChannel(channelName);
    }
    info(client.username() + " joined channel " + channelName);
}

void ChannelManager::leaveChannel(Client& client, const std::string& channelName) {
    channels_t::iterator it = _channels.find(channelName);
    if (it == _channels.end())
    {
        // we need to send an ERR 403 here
        warning("Channel " + channelName + " does not exist");
        throw ChannelManager::ChannelNonExistentException();
    }
    Channel* channel = it->second;
    std::vector<Client*>& channelClients = channel->getClients(); // by reference

    std::vector<Client*>::iterator clientIt = std::find(channelClients.begin(), channelClients.end(), &client);
    if (clientIt != channelClients.end()) {
        std::string PARTmsg = CMD_STD_FMT(client) + " PART " + channelName + " :bye!" + "\r\n";
        sendMSG(client.getFd(), PARTmsg);
        
        if (channel->isClientChanOp(&client))
            channel->removeChanOp(&client);
        info(client.username() + " removed from channel " + channelName);
        client.leaveChannel(channelName);
        channelClients.erase(clientIt);

        sendMSG(client.getFd(), RPL_NOTINCHANNEL(client, channelName));
    }
    if (channel->getClients().empty()) {
        deleteChannel(channelName);
    }
}