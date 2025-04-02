#pragma once
#include "irc.hpp"

class Channel
{
    private:
        /* member variables */
        std::vector<Client *>   _channelClients;
        std::vector<Client *>   _channelOperators;
        std::string             _channelName;
        std::string             _channelPassword;
        std::string             _channelTopic;
        std::string             _channelMode;
        size_t                  _channelClientCount;
        size_t                  _channelClientLimit;
        

    public:
        /* construcotrs & destructors */
        Channel(void);
        Channel(const std::string name);
        Channel(const Channel &other);
        ~Channel(void);
        
        /* accessors */
        std::vector<Client*>&       getClients(void);
        std::vector<Client*>&       getOperators(void);
        std::string                 getName(void) const;
        std::string                 getPassword(void) const;
        std::string                 getTopic(void) const;
        std::string                 getMode(void) const;
        size_t                      getClientCount(void) const;
        size_t                      getClientLimit(void) const;

        void                        setName(std::string name);
        void                        setPassword(std::string password);
        void                        setTopic(std::string topic);
        void                        setMode(std::string mode);
        
        /* member functions */
        bool    isEmpty(void) const;
        bool    hasClient(Client* client) const;
        void    addChanOp(Client* client);
        void    removeChanOp(Client* client);
        bool    isClientChanOp(Client* client) const;
        void    broadcastToChannel(std::string message, Client* client);        
};
