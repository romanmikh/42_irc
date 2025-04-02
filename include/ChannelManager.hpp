#pragma once
#include "irc.hpp"

class Channel;
class Client;

class ChannelManager
{
    protected:
    
    public:
        typedef std::map<std::string, Channel *>    channels_t;
        typedef std::pair<std::string, Channel *>   channel_pair_t;

        /* construcotrs & destructors */
        ChannelManager(void);
        // ChannelManager(Server& server);
        ChannelManager(const std::string name);
        ChannelManager(const ChannelManager &other);
        ~ChannelManager(void);
        
        /* accessors */
        const channels_t&       getChannels(void) const;
        clients_t               getClients(void) const;

        /* member variables */
        // Server& server;

        /* member functions */
        void    createChannel(std::string channelName);
        void    deleteChannel(std::string channelName);

        void    removeFromChannel(Client& client, std::string channelName);
        void    joinChannel(Client& client, std::string channelName);

        /* nested classes */
        class ChannelNonExistentException: public std::exception {};
        class ChannelAlreadyExistsException: public std::exception {};

    private:       

        /* member variables */
        channels_t         _channels;
        size_t             _channelCount;
        
        /* member functions */  

};

/* non-member functions & operators */