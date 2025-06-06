#pragma once

/* Standard Input/Output */
#include <iostream>

/* Standard Library */
#include <cstring>

/* String & Stream */
#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <fstream>

/* Algorithms & Utilities */
#include <iterator>
#include <algorithm>

/* Containers */
#include <map>
#include <list>
#include <vector>

/* Exception Handling */
#include <limits>
#include <signal.h>
#include <stdexcept>
#include <exception>

/* Time */
#include <ctime>

/* System & Networking */
#include <poll.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"
#include "MsgHandler.hpp"
#include "ChannelManager.hpp"

/* Macros */
#define MIN_PORT 1024
#define MAX_PORT 65535
#define SERVER_NAME std::string("42irc.local")

/* Error messages */
#define ERR_USAGE "Usage: ./ircserv <port> <password>"
#define ERR_PORT "Invalid port number"
#define ERR_PORT_NUMERIC "Port number must be numeric (1024 - 65535)"
#define ERR_INVALID_PASSWORD "Invalid password"
#define ERR_PASSWORD_FORMAT "Password must be between 4-6 characters long"
#define ERR_NOSUCHNICK(client, nick) std::string(":") + SERVER_NAME + " 401 " + client.nickname() + " " + nick + " :No such nick\r\n"
#define ERR_NOSUCHCHANNEL(client, channelName) std::string(":") + SERVER_NAME + " 403 " + client.nickname() + " " + channelName + " :No such channel\r\n"
#define ERR_NORECIPIENT(client, command) std::string(":") + SERVER_NAME + " 411 " + client.nickname() + " :No recipient given (" + command + ")\r\n"   
#define ERR_MSGTOOLONG(client, message) std::string(":") + SERVER_NAME + " 414 " + client.nickname() + " :Message is too long\r\n"
#define ERR_NONICKNAMEGIVEN(client) std::string(":") + SERVER_NAME + " 431 " + client.nickname() + " :No nickname given\r\n"
#define ERR_NICKNAMEINUSE(client, newNickname) std::string(":") + SERVER_NAME + " 433 " + client.nickname() + " " + newNickname + " :Nickname already in use\r\n"
#define ERR_NOTONCHANNEL(client, channelName) std::string(":") + SERVER_NAME + " 442 " + client.nickname() + " " + channelName + " : You're not in that channel\r\n"
#define ERR_NOTREGISTERED(client) std::string(":") + SERVER_NAME + " 451 " + client.nickname() + " :You have not registered\r\n"
#define ERR_NEEDMOREPARAMS(client, command) std::string(":") + SERVER_NAME + " 461 " + client.nickname() + " " + command + " :Not enough parameters\r\n"
#define ERR_PASSWDMISMATCH(client) std::string(":") + SERVER_NAME + " 464 " + client.nickname() + " :Password incorrect\r\n"
#define ERR_CHANNELISFULL(client, channelName) std::string(":") + SERVER_NAME + " 471 " + client.nickname() + " " + channelName + " :Cannot join channel (+l)\r\n"
#define ERR_UNKNOWNMODE(client, c) std::string(":") + SERVER_NAME + " 472 " + client.nickname() + " " + c + " : is unknown mode char to me\r\n"
#define ERR_INVITEONLYCHAN(client, channelName) std::string(":") + SERVER_NAME + " 473 " + client.nickname() + " " + channelName + " :Cannot join channel (+i)\r\n"
#define ERR_BADCHANNELKEY(client, channelName) std::string(":") + SERVER_NAME + " 475 " + client.nickname() + " " + channelName + " :Cannot join channel (+k)\r\n"
#define ERR_QUOTEBOTCONNECTING(client) std::string(":") + SERVER_NAME + client.nickname() + " QuoteBot is busy right now. Try again later\r\n"
#define ERR_BADCHANMASK(client, channelName) std::string(":") + SERVER_NAME + " 476 " + client.nickname() + " " + channelName + " :Bad Channel Mask\r\n"
#define ERR_NOPRIVILAGES(client) std::string(":") + SERVER_NAME + " 481 " + client.nickname() + ": :Permission Denied- You're not an IRC operator\r\n"
#define ERR_CHANOPPROVSNEEDED(client, channelName) std::string(":") + SERVER_NAME + " 482 " + client.nickname() + " " + channelName + " : You're not a channel operator\r\n"
#define ERR_NOOPERHOST(client) std::string(":") + SERVER_NAME + " 491 " + client.nickname() + " :No O-lines for your host\r\n"

#define RPL_WELCOME(client) std::string(":") + SERVER_NAME + " 001 " + client.nickname() + " :Welcome to the IRC Network, " + client.nickname() + "!" + client.username() + "@" + client.hostname() + "\r\n"
#define RPL_YOURHOST(client) std::string(":") + SERVER_NAME + " 002 " + client.nickname() + " :Your host is " + SERVER_NAME + ", running version 1.0\r\n"
#define RPL_CREATED(client) std::string(":") + SERVER_NAME + " 003 " + client.nickname() + " :This server was created, 2025-03-31\r\n"
#define RPL_MYINFO(client) std::string(":") + SERVER_NAME + " 004 " + client.nickname() + " " + SERVER_NAME + " 1.0 o itkol\r\n"
#define RPL_REGISTERED(client) std::string(":") + SERVER_NAME + client.nickname() + " You're registered now\r\n"
#define RPL_NOTOPIC(client, channelName) std::string(":") + SERVER_NAME + " 331 " + client.nickname() + " " + channelName + " :No topic is set\r\n"
#define RPL_TOPIC(client, channelName, topic) std::string(":") + SERVER_NAME + " 332 " + client.nickname() + " " + channelName + " :" + topic + "\r\n"
#define RPL_TOPICWHOTIME(client, channelName, nick, setAt) std::string(":") + SERVER_NAME + " 333 " + client.nickname() + " " + channelName + " " + nick + " " + setAt + "\r\n"
#define RPL_INVITING(client, nickname, channel) std::string(":") + SERVER_NAME + " 341 " + client.nickname() + " " + nickname + " " + channel + "\r\n"
#define RPL_YOUROPER(client) std::string(":") + SERVER_NAME + " 381 " + client.nickname() + " :You are now an IRC operator\r\n"
#define RPL_NOTINCHANNEL(client, channel) std::string(":") + SERVER_NAME + " 442 " + client.nickname() + " " + channel + " :You're not on that channel\r\n"
#define KILL(killer, victim, channel, reason) std::string(":") + killer.nickname() + " KILL " + victim.nickname() + " :" + reason + " (killed by " + killer.nickname() + ")\r\n"
#define QUITKILLEDBY(client, killer, reason) std::string(":") + client.nickname() + "!" + client.username() + "@" + client.hostname() + " QUIT :Killed by " + killer.nickname() + " (" + reason + ")\r\n"
#define QUIT(client, message) std::string(":") + client.nickname() + "!" + client.username() + "@" + client.hostname() + " QUIT :" + message + "\r\n"
#define DIE(client) std::string(":") + client.nickname() + "!" + client.username() + "@" + client.hostname() + " DIE: server terminated\r\n"
#define STD_PREFIX(client) std::string(":") + client.nickname() + "!" + client.username() + "@" + client.hostname()
#define PONG std::string("PONG ") + SERVER_NAME + "\r\n"
#define KICK(kicker, channel, client, reason) std::string(":") + kicker.nickname() + "!" + kicker.username() + "@" + kicker.hostname() + " KICK " + channel + " " + client + " :" + reason + "\r\n"
#define INVITE(client, nickname, channel) std::string(":") + client.nickname() + "!" + client.username() + "@" + client.hostname() + " INVITE " + nickname + " :" + channel + "\r\n"
#define JOIN(client, nickname, channel) std::string(":") + client.nickname() + "!" + client.username() + "@" + client.hostname() + " JOIN " + " :" + channel + "\r\n"
#define PART(client, channelName) std::string(":") + client.nickname() + "!" + client.username() + "@" + client.hostname() + " PART " + channelName + "\r\n"
#define PRIVMSG(client, channelName, message) std::string(":") + client.nickname() + "!" + client.username() + "@" + client.hostname() + " PRIVMSG " + channelName + " :" + message + "\r\n"
#define ADDOP(client, recipient, channelName) std::string(":") + client.nickname() + "!" + client.username() + "@" + client.hostname() + " PRIVMSG " + channelName + " :" + "Client " + recipient + " added as " + _channelName + " channel Operator by " + client.nickname() + "\r\n"
#define NOTICE(client, message) std::string(":") + SERVER_NAME + " " + client + " NOTICE : " + message + "\r\n"
#define QUOTEGREETING(channelName) std::string(":") + "QuoteBotAPI!QuoteBot@api.forismatic.com PRIVMSG " + channelName + " :QuoteBot is here to help you! Just type !quote\r\n"

/* Structures */
typedef std::pair<int, Client *>	client_pair_t;
typedef std::map<int, Client *>		clients_t;

/* Function prototypes */ 

std::vector<std::string>    split(const std::string& str, char delimiter);
std::string                 getCurrentTime(void);
std::string                 intToString(int value);
std::string                 uintToString(unsigned int value);
std::string                 boolToString(bool value);
std::string                 sizeToString(size_t value);
unsigned int                getUnsignedInt(const std::string& prompt);
int                         checkInput(int ac, char **av);
int                         errMsgVal(int detail, const std::string& str, int code);
int                         errMsg(const std::string& detail, const std::string& str, int code);
void	                    handleCtrlD(void);
int                         isDigits(const std::string& s);
int                         isValidPort(const std::string& s);
void                        printStr(const std::string& text, const std::string& colour);
void 			            sendMSG(int fd, std::string RPL);
int                         isValidPassword(const std::string& pwd);

// Logging
void                        info(const std::string& message);
void                        error(const std::string& message);
void                        warning(const std::string& message);

//Command
enum Command
{
    USER,
    NICK,
    JOIN,
    PART,
    INVITE,
    KICK,
    MODE,
    TOPIC,
    PING,
    QUIT,
    OPER,
    PRIVMSG,
    PASS,
    UNKNOWN,
    KILL,
    DIE
};

std::map<std::string, Command>  createCommandMap();
Command getCommandType(const std::string& cmd);

/* Colours */
#define RESET   "\e[0m"
#define RED     "\e[31m"
#define GREEN   "\e[32m"
#define YELLOW  "\e[33m"
#define BLUE    "\e[34m"
#define PURPLE  "\e[35m"
#define CYAN    "\e[36m"
#define WHITE   "\e[37m"
#define BRED    "\e[1;31m"
#define BGREEN  "\e[1;32m"
#define BYELLOW "\e[1;33m"
#define BBLUE   "\e[1;34m"
#define BPURPLE "\e[1;35m"
#define BCYAN   "\e[1;36m"
#define BGR     "\e[41m"
#define BGG     "\e[42m"
#define BGY     "\e[43m"
#define BGB     "\e[44m"
#define BGP     "\e[45m"
#define BGC     "\e[46m"
#define BGW     "\e[47m"
