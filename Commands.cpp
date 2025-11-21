#include "ft_irc.hpp"

void Server::processMessage(int fd, const Message& msg)
{
    std::string cmd = toUpper(msg.getCommand());
    
    Client& client = getClient(fd);
    
    if (cmd == "CAP")
        handleCap(fd, msg);
    else if (cmd == "PASS")
        handlePass(fd, msg);
    else if (cmd == "NICK")
        handleNick(fd, msg);
    else if (cmd == "USER")
        handleUser(fd, msg);
    else if (cmd == "QUIT")
        handleQuit(fd, msg);
    else if (cmd == "AUTHENTICATE" || cmd == "SA")
        return;
    else if (!client.getIsRegistered())
    {
        sendMessage(fd, ERR_NOTREGISTERED(client.getNick().empty() ? "*" : client.getNick()));
    }
    else if (cmd == "JOIN")
        handleJoin(fd, msg);
    else if (cmd == "PART")
        handlePart(fd, msg);
    else if (cmd == "PRIVMSG")
        handlePrivmsg(fd, msg);
    else if (cmd == "KICK")
        handleKick(fd, msg);
    else if (cmd == "INVITE")
        handleInvite(fd, msg);
    else if (cmd == "TOPIC")
        handleTopic(fd, msg);
    else if (cmd == "MODE")
        handleMode(fd, msg);
    else if (cmd == "WHO")
        handleWho(fd, msg);
    else if (cmd == "WHOIS")
        return;  // Silently ignore WHOIS for now
    else if (cmd == "NAMES")
        return;  // Already handled during JOIN
    else if (cmd == "PING")
        sendMessage(fd, ":irc.local PONG irc.local :" + (msg.getParamCount() > 0 ? msg.getParam(0) : "") + "\r\n");
    else if (cmd == "HELP")
        handleHelp(fd, msg);
    else
    {
        if (msg.getParamCount() == 0 && cmd.length() < 20)
            return;
        sendMessage(fd, ERR_UNKNOWNCOMMAND(client.getNick(), cmd));
    }
}

void Server::handlePass(int fd, const Message& msg)
{
    Client& client = getClient(fd);
    
    if (client.getIsRegistered())
    {
        sendMessage(fd, ERR_ALREADYREGISTRED(client.getNick()));
        return;
    }
    
    if (msg.getParamCount() < 1)
    {
        sendMessage(fd, ERR_NEEDMOREPARAMS("*", "PASS"));
        return;
    }
    
    if (msg.getParam(0) == password)
    {
        client.setPass(true);
    }
    else
    {
        sendMessage(fd, ERR_PASSWDMISMATCH("*"));
        client.setPass(false);
    }
}

void Server::handleNick(int fd, const Message& msg)
{
    Client& client = getClient(fd);

    if (!password.empty() && !client.getHasPassed())
    {
        sendMessage(fd, ERR_PASSWDMISMATCH("*"));
        return;
    }
    
    if (msg.getParamCount() < 1)
    {
        sendMessage(fd, ERR_NONICKNAMEGIVEN("*"));
        return;
    }
    
    std::string newNick = msg.getParam(0);
    
    if (!isValidNickname(newNick))
    {
        sendMessage(fd, ERR_ERRONEUSNICKNAME("*", newNick));
        return;
    }
    
    if (hasNick(newNick) && getFdByNick(newNick) != fd)
    {
        sendMessage(fd, ERR_NICKNAMEINUSE("*", newNick));
        return;
    }
    
    std::string oldNick = client.getNick();
    client.setNick(newNick);
    
    if (client.getIsRegistered() && oldNick.empty())
    {
        sendMessage(fd, RPL_WELCOME(newNick, client.getUser(), client.getHostname()));
        sendMessage(fd, RPL_YOURHOST(newNick));
        sendMessage(fd, RPL_CREATED(newNick));
        sendMessage(fd, RPL_MYINFO(newNick));
        sendMessage(fd, ":irc.local NOTICE " + newNick + " :*** You are now registered! ***");
        sendMessage(fd, ":irc.local NOTICE " + newNick + " :*** Connected to IRC Server v1.0 ***");
        sendMessage(fd, ":irc.local NOTICE " + newNick + " :Type /HELP for a list of available commands.");
        sendMessage(fd, ":irc.local NOTICE " + newNick + " :To join a channel: /JOIN #channelname");
        sendMessage(fd, ":irc.local NOTICE " + newNick + " :To send a message: /MSG #channel message");
    }
}

void Server::handleUser(int fd, const Message& msg)
{
    Client& client = getClient(fd);

    if (!password.empty() && !client.getHasPassed())
    {
        sendMessage(fd, ERR_PASSWDMISMATCH("*"));
        return;
    }
    
    if (client.getIsRegistered())
    {
        sendMessage(fd, ERR_ALREADYREGISTRED(client.getNick()));
        return;
    }
    
    if (msg.getParamCount() < 4)
    {
        sendMessage(fd, ERR_NEEDMOREPARAMS("*", "USER"));
        return;
    }
    
    client.setUser(msg.getParam(0), msg.getParam(3));
    
    if (client.getIsRegistered())
    {
        std::string nick = client.getNick();
        sendMessage(fd, RPL_WELCOME(nick, client.getUser(), client.getHostname()));
        sendMessage(fd, RPL_YOURHOST(nick));
        sendMessage(fd, RPL_CREATED(nick));
        sendMessage(fd, RPL_MYINFO(nick));
        sendMessage(fd, ":irc.local NOTICE " + nick + " :*** You are now registered! ***");
        sendMessage(fd, ":irc.local NOTICE " + nick + " :*** Connected to IRC Server v1.0 ***");
        sendMessage(fd, ":irc.local NOTICE " + nick + " :Type /HELP for a list of available commands.");
        sendMessage(fd, ":irc.local NOTICE " + nick + " :To join a channel: /JOIN #channelname");
        sendMessage(fd, ":irc.local NOTICE " + nick + " :To send a message: /MSG #channel message");
    }
}

void Server::handleQuit(int fd, const Message& msg)
{
    Client& client = getClient(fd);
    std::string reason = "Client quit";
    
    if (msg.getParamCount() > 0)
        reason = msg.getParam(0);
    
    for (std::map<std::string, Channel*>::iterator it = channels.begin();
         it != channels.end(); ++it)
    {
        if (it->second->isMember(fd))
        {
            std::string quitMsg = ":" + client.getPrefix() + " QUIT :" + reason + "\r\n";
            broadcastToChannel(it->first, quitMsg, fd);
        }
    }
    
    disconnect(fd);
}

void Server::handleJoin(int fd, const Message& msg)
{
    Client& client = getClient(fd);
    
    if (msg.getParamCount() < 1)
    {
        sendMessage(fd, ERR_NEEDMOREPARAMS(client.getNick(), "JOIN"));
        return;
    }
    
    std::string channelName = msg.getParam(0);
    std::string key = msg.getParamCount() > 1 ? msg.getParam(1) : "";
    
    if (!isValidChannelName(channelName))
    {
        sendMessage(fd, ERR_NOSUCHCHANNEL(client.getNick(), channelName));
        return;
    }
    
    Channel* channel = getChannel(channelName);
    bool isNewChannel = (channel == NULL);
    
    if (isNewChannel)
        channel = createChannel(channelName);
    
    if (channel->isMember(fd))
        return;
    
    if (channel->isInviteOnly() && !channel->isInvited(client.getNick()))
    {
        sendMessage(fd, ERR_INVITEONLYCHAN(client.getNick(), channelName));
        return;
    }
    
    if (channel->hasKeySet() && channel->getKey() != key)
    {
        sendMessage(fd, ERR_BADCHANNELKEY(client.getNick(), channelName));
        return;
    }
    
    if (channel->hasUserLimitSet() && channel->getMemberCount() >= channel->getUserLimit())
    {
        sendMessage(fd, ERR_CHANNELISFULL(client.getNick(), channelName));
        return;
    }
    
    channel->addMember(fd);
    
    if (isNewChannel)
        channel->addOperator(fd);
    
    channel->removeInvite(client.getNick());
    
    std::string joinMsg = ":" + client.getPrefix() + " JOIN :" + channelName + "\r\n";
    broadcastToChannel(channelName, joinMsg, -1);
    
    if (isNewChannel)
    {
        sendMessage(fd, ":irc.local NOTICE " + client.getNick() + " :*** Welcome to " + channelName + "! ***");
        sendMessage(fd, ":irc.local NOTICE " + client.getNick() + " :You are now the channel operator.");
        sendMessage(fd, ":irc.local NOTICE " + client.getNick() + " :Use TOPIC to set a topic: TOPIC " + channelName + " :Your topic here");
        sendMessage(fd, ":irc.local NOTICE " + client.getNick() + " :Use MODE to set channel modes: MODE " + channelName + " +t");
        sendMessage(fd, ":irc.local NOTICE " + client.getNick() + " :Type HELP MODE for more information.");
    }
    
    if (channel->getTopic().empty())
        sendMessage(fd, RPL_NOTOPIC(client.getNick(), channelName));
    else
        sendMessage(fd, RPL_TOPIC(client.getNick(), channelName, channel->getTopic()));
    
    std::string names;
    const std::set<int>& members = channel->getMembers();
    for (std::set<int>::const_iterator it = members.begin(); it != members.end(); ++it)
    {
        if (channel->isOperator(*it))
            names += "@";
        names += getNickByFd(*it) + " ";
    }
    
    sendMessage(fd, RPL_NAMREPLY(client.getNick(), channelName, names));
    sendMessage(fd, RPL_ENDOFNAMES(client.getNick(), channelName));
}

void Server::handlePart(int fd, const Message& msg)
{
    Client& client = getClient(fd);
    
    if (msg.getParamCount() < 1)
    {
        sendMessage(fd, ERR_NEEDMOREPARAMS(client.getNick(), "PART"));
        return;
    }
    
    std::string channelName = msg.getParam(0);
    std::string reason = msg.getParamCount() > 1 ? msg.getParam(1) : "Leaving";
    
    Channel* channel = getChannel(channelName);
    
    if (!channel)
    {
        sendMessage(fd, ERR_NOSUCHCHANNEL(client.getNick(), channelName));
        return;
    }
    
    if (!channel->isMember(fd))
    {
        sendMessage(fd, ERR_NOTONCHANNEL(client.getNick(), channelName));
        return;
    }
    
    std::string partMsg = ":" + client.getPrefix() + " PART " + channelName + " :" + reason + "\r\n";
    broadcastToChannel(channelName, partMsg, -1);
    
    channel->removeMember(fd);
    
    if (channel->getMemberCount() == 0)
        deleteChannel(channelName);
}

void Server::handlePrivmsg(int fd, const Message& msg)
{
    Client& client = getClient(fd);
    
    if (msg.getParamCount() < 2)
    {
        sendMessage(fd, ERR_NEEDMOREPARAMS(client.getNick(), "PRIVMSG"));
        return;
    }
    
    std::string target = msg.getParam(0);
    std::string message = msg.getParam(1);
    
    if (target[0] == '#')
    {
        Channel* channel = getChannel(target);
        
        if (!channel)
        {
            sendMessage(fd, ERR_NOSUCHCHANNEL(client.getNick(), target));
            return;
        }
        
        if (!channel->isMember(fd))
        {
            sendMessage(fd, ERR_CANNOTSENDTOCHAN(client.getNick(), target));
            return;
        }
        
        std::string privmsg = ":" + client.getPrefix() + " PRIVMSG " + target + " :" + message + "\r\n";
        broadcastToChannel(target, privmsg, fd);
    }
    else
    {
        int targetFd = getFdByNick(target);
        
        if (targetFd == -1)
        {
            sendMessage(fd, ERR_NOSUCHNICK(client.getNick(), target));
            return;
        }
        
        std::string privmsg = ":" + client.getPrefix() + " PRIVMSG " + target + " :" + message + "\r\n";
        sendMessage(targetFd, privmsg);
    }
}

void Server::handleKick(int fd, const Message& msg)
{
    Client& client = getClient(fd);
    
    if (msg.getParamCount() < 2)
    {
        sendMessage(fd, ERR_NEEDMOREPARAMS(client.getNick(), "KICK"));
        return;
    }
    
    std::string channelName = msg.getParam(0);
    std::string targetNick = msg.getParam(1);
    std::string reason = msg.getParamCount() > 2 ? msg.getParam(2) : client.getNick();
    
    Channel* channel = getChannel(channelName);
    
    if (!channel)
    {
        sendMessage(fd, ERR_NOSUCHCHANNEL(client.getNick(), channelName));
        return;
    }
    
    if (!channel->isMember(fd))
    {
        sendMessage(fd, ERR_NOTONCHANNEL(client.getNick(), channelName));
        return;
    }
    
    if (!channel->isOperator(fd))
    {
        sendMessage(fd, ERR_CHANOPRIVSNEEDED(client.getNick(), channelName));
        return;
    }
    
    int targetFd = getFdByNick(targetNick);
    
    if (targetFd == -1 || !channel->isMember(targetFd))
    {
        sendMessage(fd, ERR_USERNOTINCHANNEL(client.getNick(), targetNick, channelName));
        return;
    }
    
    std::string kickMsg = ":" + client.getPrefix() + " KICK " + channelName + " " + targetNick + " :" + reason + "\r\n";
    broadcastToChannel(channelName, kickMsg, -1);
    
    channel->removeMember(targetFd);
    
    if (channel->getMemberCount() == 0)
        deleteChannel(channelName);
}

void Server::handleInvite(int fd, const Message& msg)
{
    Client& client = getClient(fd);
    
    if (msg.getParamCount() < 2)
    {
        sendMessage(fd, ERR_NEEDMOREPARAMS(client.getNick(), "INVITE"));
        return;
    }
    
    std::string targetNick = msg.getParam(0);
    std::string channelName = msg.getParam(1);
    
    Channel* channel = getChannel(channelName);
    
    if (!channel)
    {
        sendMessage(fd, ERR_NOSUCHCHANNEL(client.getNick(), channelName));
        return;
    }
    
    if (!channel->isMember(fd))
    {
        sendMessage(fd, ERR_NOTONCHANNEL(client.getNick(), channelName));
        return;
    }
    
    if (channel->isInviteOnly() && !channel->isOperator(fd))
    {
        sendMessage(fd, ERR_CHANOPRIVSNEEDED(client.getNick(), channelName));
        return;
    }
    
    int targetFd = getFdByNick(targetNick);
    
    if (targetFd == -1)
    {
        sendMessage(fd, ERR_NOSUCHNICK(client.getNick(), targetNick));
        return;
    }
    
    if (channel->isMember(targetFd))
    {
        sendMessage(fd, ERR_USERONCHANNEL(client.getNick(), targetNick, channelName));
        return;
    }
    
    channel->addInvite(targetNick);
    
    sendMessage(fd, RPL_INVITING(client.getNick(), targetNick, channelName));
    
    std::string inviteMsg = ":" + client.getPrefix() + " INVITE " + targetNick + " " + channelName + "\r\n";
    sendMessage(targetFd, inviteMsg);
}

void Server::handleTopic(int fd, const Message& msg)
{
    Client& client = getClient(fd);
    
    if (msg.getParamCount() < 1)
    {
        sendMessage(fd, ERR_NEEDMOREPARAMS(client.getNick(), "TOPIC"));
        return;
    }
    
    std::string channelName = msg.getParam(0);
    Channel* channel = getChannel(channelName);
    
    if (!channel)
    {
        sendMessage(fd, ERR_NOSUCHCHANNEL(client.getNick(), channelName));
        return;
    }
    
    if (!channel->isMember(fd))
    {
        sendMessage(fd, ERR_NOTONCHANNEL(client.getNick(), channelName));
        return;
    }
    
    if (msg.getParamCount() == 1)
    {
        if (channel->getTopic().empty())
            sendMessage(fd, RPL_NOTOPIC(client.getNick(), channelName));
        else
            sendMessage(fd, RPL_TOPIC(client.getNick(), channelName, channel->getTopic()));
        return;
    }
    
    if (channel->isTopicRestricted() && !channel->isOperator(fd))
    {
        sendMessage(fd, ERR_CHANOPRIVSNEEDED(client.getNick(), channelName));
        return;
    }
    
    std::string newTopic = msg.getParam(1);
    channel->setTopic(newTopic);
    
    std::string topicMsg = ":" + client.getPrefix() + " TOPIC " + channelName + " :" + newTopic + "\r\n";
    broadcastToChannel(channelName, topicMsg, -1);
}

void Server::handleMode(int fd, const Message& msg)
{
    Client& client = getClient(fd);
    
    if (msg.getParamCount() < 1)
    {
        sendMessage(fd, ERR_NEEDMOREPARAMS(client.getNick(), "MODE"));
        return;
    }
    
    std::string target = msg.getParam(0);
    
    if (target[0] == '#')
    {
        Channel* channel = getChannel(target);
        
        if (!channel)
        {
            sendMessage(fd, ERR_NOSUCHCHANNEL(client.getNick(), target));
            return;
        }
        
        if (!channel->isMember(fd))
        {
            sendMessage(fd, ERR_NOTONCHANNEL(client.getNick(), target));
            return;
        }
        
        if (msg.getParamCount() == 1)
        {
            sendMessage(fd, RPL_CHANNELMODEIS(client.getNick(), target, channel->getModeString()));
            return;
        }
        
        if (!channel->isOperator(fd))
        {
            sendMessage(fd, ERR_CHANOPRIVSNEEDED(client.getNick(), target));
            return;
        }
        
        std::string modeString = msg.getParam(1);
        size_t paramIndex = 2;
        bool adding = true;
        
        for (size_t i = 0; i < modeString.size(); ++i)
        {
            char mode = modeString[i];
            
            if (mode == '+')
                adding = true;
            else if (mode == '-')
                adding = false;
            else if (mode == 'i')
                channel->setInviteOnly(adding);
            else if (mode == 't')
                channel->setTopicRestricted(adding);
            else if (mode == 'k')
            {
                if (adding && paramIndex < msg.getParamCount())
                {
                    channel->setKey(msg.getParam(paramIndex));
                    paramIndex++;
                }
                else if (!adding)
                    channel->removeKey();
            }
            else if (mode == 'o')
            {
                if (paramIndex < msg.getParamCount())
                {
                    std::string targetNick = msg.getParam(paramIndex);
                    int targetFd = getFdByNick(targetNick);
                    
                    if (targetFd != -1 && channel->isMember(targetFd))
                    {
                        if (adding)
                            channel->addOperator(targetFd);
                        else
                            channel->removeOperator(targetFd);
                    }
                    paramIndex++;
                }
            }
            else if (mode == 'l')
            {
                if (adding && paramIndex < msg.getParamCount())
                {
                    int limit = atoi(msg.getParam(paramIndex).c_str());
                    if (limit > 0)
                        channel->setUserLimit(limit);
                    paramIndex++;
                }
                else if (!adding)
                    channel->removeUserLimit();
            }
        }
        
        std::string modeMsg = ":" + client.getPrefix() + " MODE " + target + " " + modeString;
        for (size_t i = 2; i < msg.getParamCount(); ++i)
            modeMsg += " " + msg.getParam(i);
        modeMsg += "\r\n";
        broadcastToChannel(target, modeMsg, -1);
    }
}

void Server::handleHelp(int fd, const Message& msg)
{
    Client& client = getClient(fd);
    std::string nick = client.getNick().empty() ? "*" : client.getNick();
    
    if (msg.getParamCount() > 0)
    {
        std::string topic = toUpper(msg.getParam(0));
        
        if (topic == "PASS")
        {
            sendMessage(fd, ":irc.local NOTICE " + nick + " :=== PASS Command ===");
            sendMessage(fd, ":irc.local NOTICE " + nick + " :Usage: PASS <password>");
            sendMessage(fd, ":irc.local NOTICE " + nick + " :Sets the connection password. Must be sent before registration.");
            sendMessage(fd, ":irc.local NOTICE " + nick + " :Example: PASS mypassword");
        }
        else if (topic == "NICK")
        {
            sendMessage(fd, ":irc.local NOTICE " + nick + " :=== NICK Command ===");
            sendMessage(fd, ":irc.local NOTICE " + nick + " :Usage: NICK <nickname>");
            sendMessage(fd, ":irc.local NOTICE " + nick + " :Sets or changes your nickname.");
            sendMessage(fd, ":irc.local NOTICE " + nick + " :Example: NICK alice");
        }
        else if (topic == "USER")
        {
            sendMessage(fd, ":irc.local NOTICE " + nick + " :=== USER Command ===");
            sendMessage(fd, ":irc.local NOTICE " + nick + " :Usage: USER <username> <mode> <unused> :<realname>");
            sendMessage(fd, ":irc.local NOTICE " + nick + " :Registers your user information.");
            sendMessage(fd, ":irc.local NOTICE " + nick + " :Example: USER alice 0 * :Alice Smith");
        }
        else if (topic == "JOIN")
        {
            sendMessage(fd, ":irc.local NOTICE " + nick + " :=== JOIN Command ===");
            sendMessage(fd, ":irc.local NOTICE " + nick + " :Usage: JOIN <#channel> [key]");
            sendMessage(fd, ":irc.local NOTICE " + nick + " :Joins a channel. Creates it if it doesn't exist.");
            sendMessage(fd, ":irc.local NOTICE " + nick + " :You become operator if you create a new channel.");
            sendMessage(fd, ":irc.local NOTICE " + nick + " :Example: JOIN #general");
            sendMessage(fd, ":irc.local NOTICE " + nick + " :Example: JOIN #private secretkey");
        }
        else if (topic == "PART")
        {
            sendMessage(fd, ":irc.local NOTICE " + nick + " :=== PART Command ===");
            sendMessage(fd, ":irc.local NOTICE " + nick + " :Usage: PART <#channel> [reason]");
            sendMessage(fd, ":irc.local NOTICE " + nick + " :Leaves a channel.");
            sendMessage(fd, ":irc.local NOTICE " + nick + " :Example: PART #general");
            sendMessage(fd, ":irc.local NOTICE " + nick + " :Example: PART #general :Goodbye everyone!");
        }
        else if (topic == "PRIVMSG")
        {
            sendMessage(fd, ":irc.local NOTICE " + nick + " :=== PRIVMSG Command ===");
            sendMessage(fd, ":irc.local NOTICE " + nick + " :Usage: PRIVMSG <target> :<message>");
            sendMessage(fd, ":irc.local NOTICE " + nick + " :Sends a message to a user or channel.");
            sendMessage(fd, ":irc.local NOTICE " + nick + " :Example: PRIVMSG #general :Hello everyone!");
            sendMessage(fd, ":irc.local NOTICE " + nick + " :Example: PRIVMSG bob :Hi Bob!");
        }
        else if (topic == "TOPIC")
        {
            sendMessage(fd, ":irc.local NOTICE " + nick + " :=== TOPIC Command ===");
            sendMessage(fd, ":irc.local NOTICE " + nick + " :Usage: TOPIC <#channel> [new topic]");
            sendMessage(fd, ":irc.local NOTICE " + nick + " :Views or sets the channel topic.");
            sendMessage(fd, ":irc.local NOTICE " + nick + " :Without parameters: views current topic");
            sendMessage(fd, ":irc.local NOTICE " + nick + " :With topic: sets new topic (operator only if +t is set)");
            sendMessage(fd, ":irc.local NOTICE " + nick + " :Example: TOPIC #general");
            sendMessage(fd, ":irc.local NOTICE " + nick + " :Example: TOPIC #general :Welcome to our channel!");
        }
        else if (topic == "MODE")
        {
            sendMessage(fd, ":irc.local NOTICE " + nick + " :=== MODE Command ===");
            sendMessage(fd, ":irc.local NOTICE " + nick + " :Usage: MODE <#channel> <+/-modes> [parameters]");
            sendMessage(fd, ":irc.local NOTICE " + nick + " :Sets or removes channel modes (operator only).");
            sendMessage(fd, ":irc.local NOTICE " + nick + " :Available modes:");
            sendMessage(fd, ":irc.local NOTICE " + nick + " :  +i : Invite-only channel");
            sendMessage(fd, ":irc.local NOTICE " + nick + " :  +t : Topic restricted to operators");
            sendMessage(fd, ":irc.local NOTICE " + nick + " :  +k <key> : Set channel password");
            sendMessage(fd, ":irc.local NOTICE " + nick + " :  +o <nick> : Give operator privileges");
            sendMessage(fd, ":irc.local NOTICE " + nick + " :  +l <limit> : Set user limit");
            sendMessage(fd, ":irc.local NOTICE " + nick + " :Example: MODE #general +t");
            sendMessage(fd, ":irc.local NOTICE " + nick + " :Example: MODE #general +k secretpass");
            sendMessage(fd, ":irc.local NOTICE " + nick + " :Example: MODE #general +o bob");
        }
        else if (topic == "KICK")
        {
            sendMessage(fd, ":irc.local NOTICE " + nick + " :=== KICK Command ===");
            sendMessage(fd, ":irc.local NOTICE " + nick + " :Usage: KICK <#channel> <nickname> [reason]");
            sendMessage(fd, ":irc.local NOTICE " + nick + " :Kicks a user from channel (operator only).");
            sendMessage(fd, ":irc.local NOTICE " + nick + " :Example: KICK #general bob");
            sendMessage(fd, ":irc.local NOTICE " + nick + " :Example: KICK #general bob :Spamming");
        }
        else if (topic == "INVITE")
        {
            sendMessage(fd, ":irc.local NOTICE " + nick + " :=== INVITE Command ===");
            sendMessage(fd, ":irc.local NOTICE " + nick + " :Usage: INVITE <nickname> <#channel>");
            sendMessage(fd, ":irc.local NOTICE " + nick + " :Invites a user to a channel.");
            sendMessage(fd, ":irc.local NOTICE " + nick + " :Required for invite-only (+i) channels.");
            sendMessage(fd, ":irc.local NOTICE " + nick + " :Example: INVITE bob #private");
        }
        else if (topic == "QUIT")
        {
            sendMessage(fd, ":irc.local NOTICE " + nick + " :=== QUIT Command ===");
            sendMessage(fd, ":irc.local NOTICE " + nick + " :Usage: QUIT [reason]");
            sendMessage(fd, ":irc.local NOTICE " + nick + " :Disconnects from the server.");
            sendMessage(fd, ":irc.local NOTICE " + nick + " :Example: QUIT");
            sendMessage(fd, ":irc.local NOTICE " + nick + " :Example: QUIT :Goodbye!");
        }
        else if (topic == "MODES")
        {
            sendMessage(fd, ":irc.local NOTICE " + nick + " :=== Channel Modes ===");
            sendMessage(fd, ":irc.local NOTICE " + nick + " :+i : Invite-only (users must be invited to join)");
            sendMessage(fd, ":irc.local NOTICE " + nick + " :+t : Topic restricted (only operators can change topic)");
            sendMessage(fd, ":irc.local NOTICE " + nick + " :+k : Channel has a password/key");
            sendMessage(fd, ":irc.local NOTICE " + nick + " :+o : Operator privileges");
            sendMessage(fd, ":irc.local NOTICE " + nick + " :+l : User limit set");
            sendMessage(fd, ":irc.local NOTICE " + nick + " :Use MODE command to set these modes.");
        }
        else
        {
            sendMessage(fd, ":irc.local NOTICE " + nick + " :Unknown help topic: " + topic);
            sendMessage(fd, ":irc.local NOTICE " + nick + " :Type HELP for a list of available commands.");
        }
    }
    else
    {
        sendMessage(fd, ":irc.local NOTICE " + nick + " :=== IRC Server Help ===");
        sendMessage(fd, ":irc.local NOTICE " + nick + " :Available Commands:");
        sendMessage(fd, ":irc.local NOTICE " + nick + " :  PASS     - Set connection password");
        sendMessage(fd, ":irc.local NOTICE " + nick + " :  NICK     - Set your nickname");
        sendMessage(fd, ":irc.local NOTICE " + nick + " :  USER     - Set user information");
        sendMessage(fd, ":irc.local NOTICE " + nick + " :  JOIN     - Join a channel");
        sendMessage(fd, ":irc.local NOTICE " + nick + " :  PART     - Leave a channel");
        sendMessage(fd, ":irc.local NOTICE " + nick + " :  PRIVMSG  - Send a message");
        sendMessage(fd, ":irc.local NOTICE " + nick + " :  TOPIC    - View/set channel topic");
        sendMessage(fd, ":irc.local NOTICE " + nick + " :  MODE     - Set channel modes");
        sendMessage(fd, ":irc.local NOTICE " + nick + " :  KICK     - Kick a user (op only)");
        sendMessage(fd, ":irc.local NOTICE " + nick + " :  INVITE   - Invite a user");
        sendMessage(fd, ":irc.local NOTICE " + nick + " :  QUIT     - Disconnect");
        sendMessage(fd, ":irc.local NOTICE " + nick + " :For detailed help: HELP <command>");
        sendMessage(fd, ":irc.local NOTICE " + nick + " :For channel modes: HELP MODES");
        sendMessage(fd, ":irc.local NOTICE " + nick + " :======================");
    }
}

void Server::handleCap(int fd, const Message& msg)
{
    Client& client = getClient(fd);
    std::string nick = client.getNick().empty() ? "*" : client.getNick();
    
    if (msg.getParamCount() < 1)
        return;
    
    std::string subCmd = toUpper(msg.getParam(0));
    
    if (subCmd == "LS")
    {
        sendMessage(fd, ":irc.local CAP " + nick + " LS :\r\n");
    }
    else if (subCmd == "REQ")
    {
        sendMessage(fd, ":irc.local CAP " + nick + " NAK :\r\n");
    }
    else if (subCmd == "END")
    {
        return;
    }
}

void Server::handleWho(int fd, const Message& msg)
{
    Client& client = getClient(fd);
    std::string mask = msg.getParamCount() > 0 ? msg.getParam(0) : "*";
    
    if (!mask.empty() && mask[0] == '#')
    {
        Channel* channel = getChannel(mask);
        if (channel && channel->isMember(fd))
        {
            const std::set<int>& members = channel->getMembers();
            for (std::set<int>::const_iterator it = members.begin(); it != members.end(); ++it)
            {
                const Client& memberClient = getClient(*it);
                std::string flags = channel->isOperator(*it) ? "@" : "";
                
                std::string whoReply = ":irc.local 352 " + client.getNick() + " " + mask + " " +
                                      memberClient.getUser() + " " + memberClient.getHostname() + 
                                      " irc.local " + memberClient.getNick() + " H" + flags + 
                                      " :0 " + memberClient.getReal() + "\r\n";
                sendMessage(fd, whoReply);
            }
        }
    }
    
    sendMessage(fd, ":irc.local 315 " + client.getNick() + " " + mask + " :End of WHO list\r\n");
}

