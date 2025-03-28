#pragma once
#include "../include/Utils.hpp"

class User
{
    protected:

    private:

        /* member variables */
        std::string _nickname;
        std::string _username;
        int         _fd;
        
        /* member functions */
        bool    _isNicknameValid(const std::string nickname) const;
        bool    _isUsernameValid(const std::string username) const;
        bool    _isOperator(void) const;

    public:

        /* construcotrs & destructors */
        User(void);
        User(const std::string name);
        User(const User &other);
        ~User(void);

        /* operator overloads */
        User & operator = (const User &other);
        
        /* accessors */
        
        /* member functions */

        /* nested classes */
};

/* non-member functions & operators */