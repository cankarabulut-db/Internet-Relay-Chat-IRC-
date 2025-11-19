#ifndef REPLIES_HPP
#define REPLIES_HPP

#include <string>
#include <sstream>

std::string RPL_WELCOME(const std::string& nick, const std::string& user, const std::string& host);
std::string RPL_YOURHOST(const std::string& nick);
std::string RPL_CREATED(const std::string& nick);
std::string RPL_MYINFO(const std::string& nick);
std::string RPL_NOTOPIC(const std::string& nick, const std::string& channel);
std::string RPL_TOPIC(const std::string& nick, const std::string& channel, const std::string& topic);
std::string RPL_NAMREPLY(const std::string& nick, const std::string& channel, const std::string& names);
std::string RPL_ENDOFNAMES(const std::string& nick, const std::string& channel);
std::string RPL_CHANNELMODEIS(const std::string& nick, const std::string& channel, const std::string& modes);
std::string RPL_INVITING(const std::string& nick, const std::string& target, const std::string& channel);

std::string ERR_NEEDMOREPARAMS(const std::string& nick, const std::string& command);
std::string ERR_ALREADYREGISTRED(const std::string& nick);
std::string ERR_PASSWDMISMATCH(const std::string& nick);
std::string ERR_NONICKNAMEGIVEN(const std::string& nick);
std::string ERR_ERRONEUSNICKNAME(const std::string& nick, const std::string& badnick);
std::string ERR_NICKNAMEINUSE(const std::string& nick, const std::string& badnick);
std::string ERR_NOSUCHNICK(const std::string& nick, const std::string& target);
std::string ERR_NOSUCHCHANNEL(const std::string& nick, const std::string& channel);
std::string ERR_CANNOTSENDTOCHAN(const std::string& nick, const std::string& channel);
std::string ERR_TOOMANYCHANNELS(const std::string& nick, const std::string& channel);
std::string ERR_NOTONCHANNEL(const std::string& nick, const std::string& channel);
std::string ERR_USERONCHANNEL(const std::string& nick, const std::string& user, const std::string& channel);
std::string ERR_NOTREGISTERED(const std::string& nick);
std::string ERR_USERNOTINCHANNEL(const std::string& nick, const std::string& target, const std::string& channel);
std::string ERR_CHANOPRIVSNEEDED(const std::string& nick, const std::string& channel);
std::string ERR_INVITEONLYCHAN(const std::string& nick, const std::string& channel);
std::string ERR_BADCHANNELKEY(const std::string& nick, const std::string& channel);
std::string ERR_CHANNELISFULL(const std::string& nick, const std::string& channel);
std::string ERR_UNKNOWNMODE(const std::string& nick, const std::string& mode);
std::string ERR_UNKNOWNCOMMAND(const std::string& nick, const std::string& command);
std::string ERR_USERSDONTMATCH(const std::string& nick);

std::string createMessage(const std::string& prefix, const std::string& command, const std::string& params);

#endif
