#pragma once
#include "irc.hpp"

class Channel
{
    private:
        /* member variables */
        std::vector<Client *>   	_channelClients;
        std::vector<Client *>   	_channelOperators;
        std::string             	_channelName;
        std::string             	_channelPassword;
        std::string             	_channelTopic;
        std::string             	_channelTopicSetAt;
        std::string             	_channelTopicSetBy;
        bool                   	 	_channelIsInviteOnly;
        bool                    	_channelIsTopicRestricted;
        bool                    	_channelIsKeyProtected;
        bool                    	_channelIsLimitRestricted;
        size_t                  	_channelClientCount;
        size_t                  	_channelClientLimit;
        

    public:
        /* construcotrs & destructors */
        Channel(const std::string name);
        ~Channel(void);
        
        /* accessors */
        bool                        isInviteOnly(void) const;
        bool                        isTopicRestricted(void) const;
        bool                        isKeyProtected(void) const;
        bool                        isLimitRestricted(void) const;

        std::vector<Client*>&       getClients(void);
        std::vector<Client*>&       getOperators(void);
        std::string                 getName(void) const;
        std::string                 getPasskey(void) const;
        std::string                 getTopic(void) const;
        std::string                 getTopicSetBy(void) const;
        std::string                 getTopicSetAt(void) const;
        size_t                      getClientLimit(void) const;
        size_t                      getClientCount(void) const;
        size_t                      incClientCount(void);
        size_t                      decClientCount(void);

        void                        setName(std::string &name);
        void                        setPassword(std::string &password);
        void                        setTopic(std::string &topic, const std::string &nickname);
        void                        setModeI(std::string &mode, Client &client);
        void                        setModeT(std::string &mode, Client &client);
        void                        setModeK(std::vector<std::string> &msgData, Client &client);
        void                        setModeO(std::vector<std::string> &msgData, Client &client, Server &server);
        void                        setModeL(std::vector<std::string> &msgData, Client &client);
        
        /* member functions */
        bool    isEmpty(void) const;
        bool    hasClient(Client* client) const;
        void    addChanOp(Client* client);
        void    removeChanOp(Client* client);
        bool    isClientChanOp(Client* client) const;
        void    broadcast(std::string message);
        void    broadcastSilent(std::string message, Client *client);
};
