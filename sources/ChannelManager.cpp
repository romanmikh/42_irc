#include "../include/ChannelManager.hpp"

// ************************************************************************** //
//                       Constructors & Desctructors                          //
// ************************************************************************** //
ChannelManager::ChannelManager(void) {
    printStr("ChannelManager(void) created", PURPLE);
}

ChannelManager::~ChannelManager(void){
    for (channels_t::iterator it = _channels.begin(); it != _channels.end(); ++it) {
        delete it->second;
    }
    _channels.clear();
    printStr("ChannelManager destroyed", PURPLE);
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

void ChannelManager::joinChannel(Client& client, std::string channelName) {
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

void ChannelManager::leaveChannel(Client& client, std::string channelName) {
    channels_t::iterator it = _channels.find(channelName);
    if (it == _channels.end()) {
        warning("Channel " + channelName + " does not exist");
        throw ChannelManager::ChannelNonExistentException();
    }
    Channel* channel = it->second;
    std::vector<Client*>& clients = channel->getClients(); // by reference
    std::vector<Client*>::iterator clientIt = std::find(clients.begin(), clients.end(), &client);
    if (clientIt != clients.end()) {
        clients.erase(clientIt);
        client.leaveChannel(channelName);
        info(client.username() + " removed from channel " + channelName);
    }
}
// ************************************************************************** //
//                             Private Functions                              //
// ************************************************************************** //

// ************************************************************************** //
//                            Non-member Functions                            //
// ************************************************************************** //