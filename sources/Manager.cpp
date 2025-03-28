#include "../include/Manager.hpp"

// ************************************************************************** //
//                       Constructors & Desctructors                          //
// ************************************************************************** //
Manager::Manager(Server& server) : server(server) {
    printStr("Manager default created! :D", PURPLE);
}

Manager::~Manager(void){
    printStr("Manager destroyed! D:", PURPLE);
}

// ************************************************************************** //
//                               Accessors                                    //
// ************************************************************************** //
Manager::channels_t      Manager::getChannels(void) const {
    return _channels;
}

// Server::clients_t       Manager::getClients(void) const {
//     return Server.getClients();
// }
// ************************************************************************** //
//                             Public Functions                               //
// ************************************************************************** //
// void    Manager::createClient(int clientFd) {
    
//     _clients.insert(client_pair_t (clientFd, new Client(clientFd)));
//     printStr("Client created! :D", PURPLE);
// }

void    Manager::createChannel(std::string channelName) {
    _channels.insert(channel_pair_t (channelName, new Channel(channelName)));
    printStr("Channel created: " + channelName, PURPLE);
}

void    Manager::deleteChannel(std::string channelName) {
    channels_t::iterator it = _channels.find(channelName);
    if (it != _channels.end()) {
        delete it->second;      // frees memory of Channel* obj
        _channels.erase(it);    // frees entry in map
        info("Channel deleted: " + channelName);
    }
    else {
        throw Manager::ChannelNonExistentException();
    }
}
// ************************************************************************** //
//                             Private Functions                              //
// ************************************************************************** //

// ************************************************************************** //
//                            Non-member Functions                            //
// ************************************************************************** //