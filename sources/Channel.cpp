#include "../include/Channel.hpp"

// ************************************************************************** //
//                       Constructors & Desctructors                          //
// ************************************************************************** //
Channel::Channel(std::string name) : _channelName(name), 
                                     _channelPassword(""),
                                     _channelTopic(""),
                                     _channelMode("") {}

Channel::~Channel(void){
    printStr("Channel destroyed", PURPLE);
}

// ************************************************************************** //
//                               Accessors                                    //
// ************************************************************************** //
std::string             Channel::getName(void) const {
    return _channelName;
}

std::string             Channel::getPassword(void) const {
    return _channelPassword;
}

std::string             Channel::getTopic(void) const {
    return _channelTopic;
}

std::string             Channel::getMode(void) const {
    return _channelMode;
}

size_t                  Channel::getClientCount(void) const {
    return _channelClientCount;
}

size_t                  Channel::getClientLimit(void) const {
    return _channelClientLimit;
}

std::vector<Client *>&  Channel::getClients(void) {
    return _channelClients;
}

std::vector<Client *>&  Channel::getOperators(void) {
    return _channelOperators;
}

void                    Channel::setName(std::string name) {
    _channelName = name;
}

void                    Channel::setPassword(std::string password) {
    _channelPassword = password;
}

void                    Channel::setTopic(std::string topic) {
    _channelTopic = topic;
}

void                    Channel::setMode(std::string mode) {
    _channelMode = mode;
}

// ************************************************************************** //
//                             Public Functions                               //
// ************************************************************************** //
bool Channel::isEmpty(void) const{
    return _channelClients.empty();
}

bool    Channel::hasClient(Client* client) const {
    for (std::vector<Client *>::const_iterator it = _channelClients.begin(); \
                                           it != _channelClients.end(); ++it) {
        if (*it == client)
            return true;
    }
    return false;
}

bool    Channel::isClientOperator(Client* client) const
{
    for (std::vector<Client *>::const_iterator it = _channelClients.begin(); it != _channelClients.end(); ++it)
    {
        if (*it == client && (*it)->isChanOp())
            return true;
    }
    return false;
}

void    Channel::addOperator(Client* client) {
    if (isClientOperator(client)) {
        warning(client->username() + " is already an operator in channel " + _channelName);
        return;
    }
    _channelOperators.push_back(client);
    info(client->username() + " is now an operator in channel " + _channelName);
}

void    Channel::removeOperator(Client* client) {
    if (client->isChanOp()) {
        warning(client->username() + " is a global operator");
        return;
    }
    if (!isClientOperator(client)) {
        warning(client->username() + " is not an operator in channel " + _channelName);
        return;
    }
    for (std::vector<Client *>::iterator it = _channelOperators.begin(); \
                                         it != _channelOperators.end(); ++it) {
        if (*it == client) {
            _channelOperators.erase(it);
            return;
        }
    }
    info(client->username() + " is no longer an operator in channel " + _channelName);
}

void    Channel::sendMessageToChannel(std::string message) {
    for (std::vector<Client *>::const_iterator it = _channelClients.begin(); \
                                           it != _channelClients.end(); ++it) {
        send((*it)->getFd(), message.c_str(), message.length(), 0);
    }
}

// ************************************************************************** //
//                             Private Functions                              //
// ************************************************************************** //

// ************************************************************************** //
//                            Non-member Functions                            //
// ************************************************************************** //