#include "../include/Utils.hpp"
#include "../include/PasswordManager.hpp"

// ************************************************************************** //
//                       Constructors & Desctructors                          //
// ************************************************************************** //
PasswordManager::PasswordManager(void){
    printStr("PasswordManager default created! :D", PURPLE);
}

PasswordManager::PasswordManager(const PasswordManager & other) {  
    *this = other;
    printStr("PasswordManager copied (deep copy unnecessary)! :D", PURPLE);
}

PasswordManager::~PasswordManager(void){
    printStr("PasswordManager destroyed! D:", PURPLE);
}

// ************************************************************************** //
//                           Operator Overloads                               //
// ************************************************************************** //
PasswordManager & PasswordManager::operator=(const PasswordManager &other) {
    (void)other;
    return *this;
}

// ************************************************************************** //
//                               Accessors                                    //
// ************************************************************************** //

// ************************************************************************** //
//                             Public Functions                               //
// ************************************************************************** //

// ************************************************************************** //
//                             Private Functions                              //
// ************************************************************************** //

// ************************************************************************** //
//                            Non-member Functions                            //
// ************************************************************************** //