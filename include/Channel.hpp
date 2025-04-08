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
        bool                   	 	_channelIsInviteOnly;
        bool                    	_channelIsTopicRestricted;
        bool                    	_channelIsKeyProtected;
        bool                    	_channelIsOperatorRestricted;
        bool                    	_channelIsLimitRestricted;
        size_t                  	_channelClientCount;
        size_t                  	_channelClientLimit;
        

    public:
        /* construcotrs & destructors */
        Channel(void);
        Channel(const std::string name);
        Channel(const Channel &other);
        ~Channel(void);
        
        /* accessors */
        bool                        isInviteOnly(void) const;
        bool                        isTopicRestricted(void) const;
        bool                        isKeyProtected(void) const;
        bool                        isOperatorRestricted(void) const;
        bool                        isLimitRestricted(void) const;

        std::vector<Client*>&       getClients(void);
        std::vector<Client*>&       getOperators(void);
        std::string                 getName(void) const;
        std::string                 getPassword(void) const;
        std::string                 getTopic(void) const;
        size_t                      getClientLimit(void) const;
        size_t                      getClientCount(void) const;
        size_t                      incClientCount(void);
        size_t                      decClientCount(void);

        void                        setName(std::string &name);
        void                        setPassword(std::string &password);
        void                        setTopic(std::string &topic);
        void                        setModeI(std::string &mode, Client &client);
        void                        setModeT(std::string &mode, Client &client);
        void                        setModeK(std::string &mode, std::string &password, Client &client);
        void                        setModeO(std::string &mode, Client &client);
        void                        setModeL(std::string &mode, Client &client);
        
        /* member functions */
        bool    isEmpty(void) const;
        bool    actionMode(std::string mode, std::string arg, Client& client);
        bool    hasClient(Client* client) const;
        void    addChanOp(Client* client);
        void    removeChanOp(Client* client);
        bool    isClientChanOp(Client* client) const;
        void    broadcastToChannel(std::string message, Client* client);        
};
