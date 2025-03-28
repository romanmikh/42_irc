#include "../include/ChannelManager.hpp"

// ************************************************************************** //
//                       Constructors & Desctructors                          //
// ************************************************************************** //
ChannelManager::ChannelManager(Server& server) : server(server) {
    printStr("ChannelManager default created! :D", PURPLE);
}

ChannelManager::~ChannelManager(void){
    printStr("ChannelManager destroyed! D:", PURPLE);
}

// ************************************************************************** //
//                               Accessors                                    //
// ************************************************************************** //
ChannelManager::channels_t      ChannelManager::getChannels(void) const {
    return _channels;
}

// Server::clients_t       ChannelManager::getClients(void) const {
//     return Server.getClients();
// }
// ************************************************************************** //
//                             Public Functions                               //
// ************************************************************************** //
// void    ChannelManager::createClient(int clientFd) {
    
//     _clients.insert(client_pair_t (clientFd, new Client(clientFd)));
//     printStr("Client created! :D", PURPLE);
// }

void    ChannelManager::createChannel(std::string channelName) {
    _channels.insert(channel_pair_t (channelName, new Channel(channelName)));
    printStr("Channel created: " + channelName, PURPLE);
}

void    ChannelManager::deleteChannel(std::string channelName) {
    channels_t::iterator it = _channels.find(channelName);
    if (it != _channels.end()) {
        delete it->second;      // frees memory of Channel* obj
        _channels.erase(it);    // frees entry in map
        info("Channel deleted: " + channelName);
    }
    else {
        throw ChannelManager::ChannelNonExistentException();
    }
}
// ************************************************************************** //
//                             Private Functions                              //
// ************************************************************************** //

// ************************************************************************** //
//                            Non-member Functions                            //
// ************************************************************************** //