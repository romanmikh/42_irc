#pragma once
#include "../include/irc.hpp"

class Channel
{
    private:
        /* member variables */
        std::vector<Client *>   _channelClients;
        std::string             _channelName;
        std::string             _channelPassword;
        std::string             _channelTopic;
        std::string             _channelMode;
        size_t                  _channelClientCount;
        size_t                  _channelClientLimit;

        /* member functions */
        void    _createChannel(std::string channelName);
        void    _deleteChannel(std::string channelName);

        bool    _isClientInChannel(std::string channelName, std::string Clientname);
        void    _addClientToChannel(std::string channelName, std::string Clientname);
        void    _removeClientFromChannel(std::string channelName, std::string Clientname);
        
        bool    _isClientOperator(std::string channelName, std::string Clientname);
        void    _makeClientOperator(std::string channelName, std::string Clientname);
        void    _removeClientOperator(std::string channelName, std::string Clientname);

        void    _sendMessageToChannel(std::string channelName, std::string message);
        bool    _isEmptyChannel(std::string channelName);


    public:
        /* construcotrs & destructors */
        Channel(void);
        Channel(const std::string name);
        Channel(const Channel &other);
        ~Channel(void);

        /* operator overloads */
        Channel & operator = (const Channel &other);
        
        /* accessors */
        std::vector<Client *>   getClients(void) const;
        std::string             getName(void) const;

        /* member functions */

};

/* non-member functions & operators */