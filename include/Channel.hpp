#pragma once
#include "../include/Utils.hpp"
#include "../include/User.hpp"

class Channel
{
    protected:

    private:


        /* member variables */
        std::string             _channelName;
        std::string             _channelPassword;

        std::vector<User *>     _channelUsers;
        size_t                  _channelUserCount;
        size_t                  _channelUserLimit;

        /* member functions */
        void    _createChannel(std::string channelName);
        void    _deleteChannel(std::string channelName);

        bool    _isUserInChannel(std::string channelName, std::string username);
        void    _addUserToChannel(std::string channelName, std::string username);
        void    _removeUserFromChannel(std::string channelName, std::string username);
        bool    _isUserOperator(std::string channelName, std::string username);
        void    _makeUserOperator(std::string channelName, std::string username);
        void    _removeUserOperator(std::string channelName, std::string username);

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
        std::vector<User *> getChannelUsers(void);

        /* member functions */

        /* nested classes */
		class ChannelNonExistentException: public std::exception {};
        class ChannelAlreadyExistsException: public std::exception {};
};

/* non-member functions & operators */