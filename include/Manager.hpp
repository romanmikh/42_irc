#pragma once
#include "../include/irc.hpp"

class Channel;

class Manager
{
    protected:
    
    public:
        typedef std::map<std::string, Channel *>    channels_t;
        typedef std::pair<std::string, Channel *>   channel_pair_t;

        /* construcotrs & destructors */
        Manager(Server& server);
        Manager(const std::string name);
        Manager(const Manager &other);
        ~Manager(void);
        
        /* accessors */
        channels_t      getChannels(void) const;
        Server::clients_t       getClients(void) const;

        /* member variables */
        Server& server;

        /* member functions */
        void    createChannel(std::string channelName);
        void    deleteChannel(std::string channelName);
        void    createClient(int clientFd);
        void    deleteClient(int clientFd);
        void    joinChannel(int clientFd, std::string channelName);
        void    leaveChannel(int clientFd, std::string channelName);

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