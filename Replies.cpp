#include "Replies.hpp"

std::string RPL_WELCOME(const std::string& nick, const std::string& user, const std::string& host) {
    return ":irc.local 001 " + nick + " :Welcome to the Internet Relay Network " + nick + "!" + user + "@" + host + "\r\n";
}

std::string RPL_YOURHOST(const std::string& nick) {
    return ":irc.local 002 " + nick + " :Your host is irc.local, running version 1.0\r\n";
}

std::string RPL_CREATED(const std::string& nick) {
    return ":irc.local 003 " + nick + " :This server was created 2025\r\n";
}

std::string RPL_MYINFO(const std::string& nick) {
    return ":irc.local 004 " + nick + " :irc.local 1.0 o itklmo\r\n";
}

std::string RPL_NOTOPIC(const std::string& nick, const std::string& channel) {
    return ":irc.local 331 " + nick + " " + channel + " :No topic is set\r\n";
}

std::string RPL_TOPIC(const std::string& nick, const std::string& channel, const std::string& topic) {
    return ":irc.local 332 " + nick + " " + channel + " :" + topic + "\r\n";
}

std::string RPL_NAMREPLY(const std::string& nick, const std::string& channel, const std::string& names) {
    return ":irc.local 353 " + nick + " = " + channel + " :" + names + "\r\n";
}

std::string RPL_ENDOFNAMES(const std::string& nick, const std::string& channel) {
    return ":irc.local 366 " + nick + " " + channel + " :End of /NAMES list\r\n";
}

std::string RPL_CHANNELMODEIS(const std::string& nick, const std::string& channel, const std::string& modes) {
    return ":irc.local 324 " + nick + " " + channel + " " + modes + "\r\n";
}

std::string RPL_INVITING(const std::string& nick, const std::string& target, const std::string& channel) {
    return ":irc.local 341 " + nick + " " + target + " " + channel + "\r\n";
}

std::string ERR_NEEDMOREPARAMS(const std::string& nick, const std::string& command) {
    return ":irc.local 461 " + nick + " " + command + " :Not enough parameters\r\n";
}

std::string ERR_ALREADYREGISTRED(const std::string& nick) {
    return ":irc.local 462 " + nick + " :You may not reregister\r\n";
}

std::string ERR_PASSWDMISMATCH(const std::string& nick) {
    return ":irc.local 464 " + nick + " :Password incorrect\r\n";
}

std::string ERR_NONICKNAMEGIVEN(const std::string& nick) {
    return ":irc.local 431 " + nick + " :No nickname given\r\n";
}

std::string ERR_ERRONEUSNICKNAME(const std::string& nick, const std::string& badnick) {
    return ":irc.local 432 " + nick + " " + badnick + " :Erroneous nickname\r\n";
}

std::string ERR_NICKNAMEINUSE(const std::string& nick, const std::string& badnick) {
    return ":irc.local 433 " + nick + " " + badnick + " :Nickname is already in use\r\n";
}

std::string ERR_NOSUCHNICK(const std::string& nick, const std::string& target) {
    return ":irc.local 401 " + nick + " " + target + " :No such nick/channel\r\n";
}

std::string ERR_NOSUCHCHANNEL(const std::string& nick, const std::string& channel) {
    return ":irc.local 403 " + nick + " " + channel + " :No such channel\r\n";
}

std::string ERR_CANNOTSENDTOCHAN(const std::string& nick, const std::string& channel) {
    return ":irc.local 404 " + nick + " " + channel + " :Cannot send to channel\r\n";
}

std::string ERR_TOOMANYCHANNELS(const std::string& nick, const std::string& channel) {
    return ":irc.local 405 " + nick + " " + channel + " :You have joined too many channels\r\n";
}

std::string ERR_NOTONCHANNEL(const std::string& nick, const std::string& channel) {
    return ":irc.local 442 " + nick + " " + channel + " :You're not on that channel\r\n";
}

std::string ERR_USERONCHANNEL(const std::string& nick, const std::string& user, const std::string& channel) {
    return ":irc.local 443 " + nick + " " + user + " " + channel + " :is already on channel\r\n";
}

std::string ERR_NOTREGISTERED(const std::string& nick) {
    return ":irc.local 451 " + nick + " :You have not registered\r\n";
}

std::string ERR_USERNOTINCHANNEL(const std::string& nick, const std::string& target, const std::string& channel) {
    return ":irc.local 441 " + nick + " " + target + " " + channel + " :They aren't on that channel\r\n";
}

std::string ERR_CHANOPRIVSNEEDED(const std::string& nick, const std::string& channel) {
    return ":irc.local 482 " + nick + " " + channel + " :You're not channel operator\r\n";
}

std::string ERR_INVITEONLYCHAN(const std::string& nick, const std::string& channel) {
    return ":irc.local 473 " + nick + " " + channel + " :Cannot join channel (+i)\r\n";
}

std::string ERR_BADCHANNELKEY(const std::string& nick, const std::string& channel) {
    return ":irc.local 475 " + nick + " " + channel + " :Cannot join channel (+k)\r\n";
}

std::string ERR_CHANNELISFULL(const std::string& nick, const std::string& channel) {
    return ":irc.local 471 " + nick + " " + channel + " :Cannot join channel (+l)\r\n";
}

std::string ERR_UNKNOWNMODE(const std::string& nick, const std::string& mode) {
    return ":irc.local 472 " + nick + " " + mode + " :is unknown mode char to me\r\n";
}

std::string ERR_UNKNOWNCOMMAND(const std::string& nick, const std::string& command) {
    return ":irc.local 421 " + nick + " " + command + " :Unknown command\r\n";
}

std::string ERR_USERSDONTMATCH(const std::string& nick) {
    return ":irc.local 502 " + nick + " :Cant change mode for other users\r\n";
}

std::string createMessage(const std::string& prefix, const std::string& command, const std::string& params)
{
    std::string msg;
    
    if (!prefix.empty())
        msg += ":" + prefix + " ";
    
    msg += command;
    
    if (!params.empty())
        msg += " " + params;
    
    msg += "\r\n";
    
    return msg;
}

