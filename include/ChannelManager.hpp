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
        Channel*                getChanByName(const std::string& channelName);

        /* member functions */
        void        createChannel(const std::string &channelName, Client *firstClient);
        void        deleteChannel(const std::string &channelName);
        void        removeFromChannel(Client& client, const std::string& channelName);
        void        kickFromChannel(std::string &msg, Client &kicker);
        void        addToChannel(Client& client, const std::string& channelName);
        bool        channelExists(const std::string& channelName) const;
        void        inviteClient(std::string &user, const std::string& chan, Client &client);
        bool        isInvited(Client& client, const std::string& channelName) const;

    private:       

        /* member variables */
        channels_t         _channels;
        size_t             _channelCount;

};
