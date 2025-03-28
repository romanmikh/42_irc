#pragma once
#include "../include/irc.hpp"

class Channel;

class Manager
{
    protected:

    private:
        typedef std::map<std::string, Channel *>    channels_t;
        typedef std::map<int, Client *>             clients_t;

        typedef std::pair<std::string, Channel *>   channel_pair_t;
        typedef std::pair<int, Client *>            client_pair_t;

        /* member variables */
        channels_t         _channels;
        clients_t          _clients;
        size_t             _channelCount;
        size_t             _clientCount;

        /* member functions */  

    public:

        /* construcotrs & destructors */
        Manager(Server& server);
        Manager(const std::string name);
        Manager(const Manager &other);
        ~Manager(void);
        
        /* accessors */
        channels_t getChannels(void) const;
        clients_t getClients(void) const;

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
};

/* non-member functions & operators */