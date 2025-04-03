#include "../include/irc.hpp"

std::map<std::string, Command> createCommandMap()
{
    std::map<std::string, Command> commandMap;
    commandMap["USER"] = USER;
    commandMap["NICK"] = NICK;
    commandMap["JOIN"] = JOIN;
    commandMap["INVITE"] = INVITE;
    commandMap["KICK"] = KICK;
    commandMap["MODE"] = MODE;
    commandMap["TOPIC"] = TOPIC;
    commandMap["PING"] = PING;
    commandMap["QUIT"] = QUIT;
    commandMap["OPER"] = OPER;
    commandMap["PASS"] = PASS;
    commandMap["PART"] = PART;
    commandMap["PRIVMSG"] = PRIVMSG;
    commandMap["KILL"] = KILL;
    commandMap["DIE"] = DIE;
    return commandMap;
}

Command getCommandType(const std::string& cmd)
{
    static const std::map<std::string, Command> commandMap = createCommandMap();
	std::map<std::string, Command>::const_iterator it = commandMap.find(cmd);

    if (it == commandMap.end())
    	return UNKNOWN;
    return it->second;
}

