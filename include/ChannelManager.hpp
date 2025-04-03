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
        ChannelManager(Server& server);
        ~ChannelManager(void);
        Server& _server;
        
        /* accessors */
        const channels_t&       getChannels(void) const;
        size_t                  getChannelCount(void) const;
        size_t                  incChannelCount(void);
        size_t                  decChannelCount(void);

        /* member functions */
        void        createChannel(std::string channelName);
        void        deleteChannel(std::string channelName);
        void        removeFromChannel(Client& client, const std::string& channelName);
        void        addToChannel(Client& client, const std::string& channelName);
        bool        channelExists(const std::string& channelName) const;
        void        inviteClient(std::string user, const std::string& chan, Client client);
        Channel*    getChanByName(const std::string& channelName);

    private:       

        /* member variables */
        channels_t         _channels;
        size_t             _channelCount;

};
