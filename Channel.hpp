#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <set>
#include <map>

class Client;

class Channel
{
private:
    std::string name;
    std::string topic;
    std::string key;
    
    std::set<int> members;         // All channel members (by fd)
    std::set<int> operators;       // Channel operators (by fd)
    std::set<std::string> inviteList; // Invited users (by nickname)
    
    bool inviteOnly;               // +i mode
    bool topicRestricted;          // +t mode
    bool hasKey;                   // +k mode
    bool hasUserLimit;             // +l mode
    size_t userLimit;

public:
    Channel(const std::string& name);
    
    void addMember(int fd);
    void removeMember(int fd);
    bool isMember(int fd) const;
    const std::set<int>& getMembers() const;
    size_t getMemberCount() const;
    
    void addOperator(int fd);
    void removeOperator(int fd);
    bool isOperator(int fd) const;
    const std::set<int>& getOperators() const;
    
    void addInvite(const std::string& nick);
    void removeInvite(const std::string& nick);
    bool isInvited(const std::string& nick) const;
    
    void setInviteOnly(bool value);
    void setTopicRestricted(bool value);
    void setKey(const std::string& newKey);
    void removeKey();
    void setUserLimit(size_t limit);
    void removeUserLimit();
    
    bool isInviteOnly() const;
    bool isTopicRestricted() const;
    bool hasKeySet() const;
    bool hasUserLimitSet() const;
    const std::string& getKey() const;
    size_t getUserLimit() const;
    
    void setTopic(const std::string& newTopic);
    const std::string& getTopic() const;
    
    const std::string& getName() const;
    
    std::string getModeString() const;
};

#endif
