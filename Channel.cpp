#include "Channel.hpp"

Channel::Channel(const std::string& name)
    : name(name), inviteOnly(false), topicRestricted(true),
      hasKey(false), hasUserLimit(false), userLimit(0)
{
}

void Channel::addMember(int fd)
{
    members.insert(fd);
}

void Channel::removeMember(int fd)
{
    members.erase(fd);
    operators.erase(fd);
}

bool Channel::isMember(int fd) const
{
    return members.find(fd) != members.end();
}

const std::set<int>& Channel::getMembers() const
{
    return members;
}

size_t Channel::getMemberCount() const
{
    return members.size();
}

void Channel::addOperator(int fd)
{
    operators.insert(fd);
}

void Channel::removeOperator(int fd)
{
    operators.erase(fd);
}

bool Channel::isOperator(int fd) const
{
    return operators.find(fd) != operators.end();
}

const std::set<int>& Channel::getOperators() const
{
    return operators;
}

void Channel::addInvite(const std::string& nick)
{
    inviteList.insert(nick);
}

void Channel::removeInvite(const std::string& nick)
{
    inviteList.erase(nick);
}

bool Channel::isInvited(const std::string& nick) const
{
    return inviteList.find(nick) != inviteList.end();
}

void Channel::setInviteOnly(bool value)
{
    inviteOnly = value;
}

void Channel::setTopicRestricted(bool value)
{
    topicRestricted = value;
}

void Channel::setKey(const std::string& newKey)
{
    key = newKey;
    hasKey = true;
}

void Channel::removeKey()
{
    key.clear();
    hasKey = false;
}

void Channel::setUserLimit(size_t limit)
{
    userLimit = limit;
    hasUserLimit = true;
}

void Channel::removeUserLimit()
{
    userLimit = 0;
    hasUserLimit = false;
}

bool Channel::isInviteOnly() const
{
    return inviteOnly;
}

bool Channel::isTopicRestricted() const
{
    return topicRestricted;
}

bool Channel::hasKeySet() const
{
    return hasKey;
}

bool Channel::hasUserLimitSet() const
{
    return hasUserLimit;
}

const std::string& Channel::getKey() const
{
    return key;
}

size_t Channel::getUserLimit() const
{
    return userLimit;
}

void Channel::setTopic(const std::string& newTopic)
{
    topic = newTopic;
}

const std::string& Channel::getTopic() const
{
    return topic;
}

const std::string& Channel::getName() const
{
    return name;
}

std::string Channel::getModeString() const
{
    std::string modes = "+";
    
    if (inviteOnly)
        modes += "i";
    if (topicRestricted)
        modes += "t";
    if (hasKey)
        modes += "k";
    if (hasUserLimit)
        modes += "l";
    
    if (modes == "+")
        return "";
    
    return modes;
}
