#include "../include/Channel.hpp"

// ************************************************************************** //
//                       Constructors & Desctructors                          //
// ************************************************************************** //
Channel::Channel(std::string name) : _channelName(name), 
                                     _channelPassword(""),
                                     _channelTopic("Default"),
                                     _channelMode("Default"),
                                     _channelIsInviteOnly(false),
                                     _channelIsTopicRestricted(false),
                                     _channelIsKeyProtected(false),
                                     _channelIsOperatorRestricted(false),
                                     _channelIsLimitRestricted(false),
                                     _channelClientCount(0),
                                     _channelClientLimit(CHAN_CLIENT_LIMIT) {}

Channel::~Channel(void){
    printStr("Channel destroyed", PURPLE);
}

// ************************************************************************** //
//                               Accessors                                    //
// ************************************************************************** //
bool                    Channel::isInviteOnly(void) const {
    return _channelIsInviteOnly;
}

bool                    Channel::isTopicRestricted(void) const {
    return _channelIsTopicRestricted;
}

bool                    Channel::isKeyProtected(void) const {
    return _channelIsKeyProtected;
}

bool                    Channel::isOperatorRestricted(void) const {
    return _channelIsOperatorRestricted;
}

bool                    Channel::isLimitRestricted(void) const {
    return _channelIsLimitRestricted;
}

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

size_t                  Channel::incClientCount(void) {
    return ++_channelClientCount;
}

size_t                  Channel::decClientCount(void) {
    if (_channelClientCount > 0)
        return --_channelClientCount;
    else {
        error("Client count is already 0");
        return _channelClientCount;
    }
}

// ************************************************************************** //
//                             Public Functions                               //
// ************************************************************************** //
bool    Channel::isEmpty(void) const {
    return _channelClients.empty();
}

bool    Channel::actionMode(std::string mode, Client& client) {
    if (mode == "+i" || mode == "-i") {
        _channelMode = mode;
        _channelIsInviteOnly = (mode == "+i");
        (void)client;
        // broadcastToChannel(client.username() + " set the mode on " + 
        //                          _channelName + "to: " + mode + "\r\n", NULL);
        info("Channel " + _channelName + " is now invite only: " + 
                                            boolToString(_channelIsInviteOnly));
        return true;
    }
    else if (mode == "+t" || mode == "-t") {
        _channelMode = mode;
        _channelIsTopicRestricted = (mode == "+t");
        info("Channel " + _channelName + " is now topic restricted: " + 
                                       boolToString(_channelIsTopicRestricted));
        return true;
    }
    else if (mode == "+k" || mode == "-k") {
        _channelMode = mode;
        _channelIsKeyProtected = (mode == "+k");
        info("Channel " + _channelName + " is now key protected: " + 
                                          boolToString(_channelIsKeyProtected));
        return true;
    }
    else if (mode == "+o" || mode == "-o") {
        _channelMode = mode;
        _channelIsOperatorRestricted = (mode == "+o");
        info("Channel " + _channelName + " is now operator restricted: " + 
                                    boolToString(_channelIsOperatorRestricted));
        return true;
    }
    else if (mode == "+l" || mode == "-l") {
        _channelMode = mode;
        _channelIsLimitRestricted = (mode == "+l");
        info("Channel " + _channelName + " is now limit restricted (currently " + 
                                    sizeToString(_channelClientCount) + "/" +
                                    sizeToString(_channelClientLimit) + "): " + 
                                    boolToString(_channelIsLimitRestricted));
        return true;
    }
    return false;
}

bool    Channel::hasClient(Client* client) const {
    for (std::vector<Client *>::const_iterator it = _channelClients.begin(); \
                                           it != _channelClients.end(); ++it) {
        if (*it == client)
            return true;
    }
    return false;
}

bool    Channel::isClientChanOp(Client* client) const {
    for (std::vector<Client *>::const_iterator it = _channelOperators.begin(); \
                                        it != _channelOperators.end(); ++it) {
        if (*it == client)
            return true;
    }
    return false;
}

void    Channel::addChanOp(Client* client) {
    if (client->isIRCOp()) {
        return warning(client->username() + " is a global operator");
    }
    if (isClientChanOp(client)) {
        return warning(client->username() + " is already an operator in channel " 
                                                                + _channelName);
    }
    _channelOperators.push_back(client);
    info(client->username() + " is now an operator in channel " + _channelName);
}

void    Channel::removeChanOp(Client* client) {
    if (client->isIRCOp()) {
        return warning(client->username() + " is a global operator");
    }
    if (!isClientChanOp(client)) {
        return warning(client->username() + " is not an operator in channel " 
                                                                + _channelName);
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

void    Channel::broadcastToChannel(std::string message, Client* client) {
    if (isEmpty()) {
        return warning("Channel is empty");
    }
    if (message.empty()) {
        return warning("Empty message");
    }
    if (message.length() > 512) {
        return warning("Message too long");
    }
    for (std::vector<Client *>::const_iterator it = _channelClients.begin(); it != _channelClients.end(); ++it)
    {
        if (*it != client)
            sendMSG((*it)->getFd(), (message + "\r\n"));
    }
}

// ************************************************************************** //
//                             Private Functions                              //
// ************************************************************************** //

// ************************************************************************** //
//                            Non-member Functions                            //
// ************************************************************************** //