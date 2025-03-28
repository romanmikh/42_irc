#pragma once
#include "../include/Utils.hpp"
#include "../include/Channel.hpp"

class Channel;

class Manager
{
    protected:

    private:
        /* member variables */
        std::map<std::string, Channel *> _channels;
        size_t                          _channelCount;
        std::map<std::string, Client *>   _clients;
        size_t                          _clientCount;

        /* member functions */
        

    public:

        /* construcotrs & destructors */
        Manager(void);
        Manager(const std::string name);
        Manager(const Manager &other);
        ~Manager(void);
        
        /* accessors */

        /* member functions */
        void    createChannel(std::string channelName);
        void    deleteChannel(std::string channelName);
        void    createClient(int clientFd);

        /* nested classes */
};

/* non-member functions & operators */