#include "Client.hpp"

Client::Client() 
    : fd(-1), port(0), hasPassed(false), hasNick(false), 
      hasUser(false), isRegistered(false)
{
}

Client::Client(int fd, const std::string& ip, int port)
    : fd(fd), ip(ip), port(port), hostname("localhost"),
      hasPassed(false), hasNick(false), hasUser(false), isRegistered(false)
{
}

// Getters
int Client::getFd() const 
{ 
    return fd; 
}

std::string Client::getNick() const 
{ 
    return nickname; 
}

std::string Client::getUser() const 
{ 
    return username; 
}

std::string Client::getReal() const 
{ 
    return realname; 
}

std::string Client::getHostname() const 
{ 
    return hostname; 
}

std::string Client::getPrefix() const
{
    if (nickname.empty())
        return "";
    return nickname + "!" + username + "@" + hostname;
}

bool Client::getHasPassed() const 
{ 
    return hasPassed; 
}

bool Client::getHasNick() const 
{ 
    return hasNick; 
}

bool Client::getHasUser() const 
{ 
    return hasUser; 
}

bool Client::getIsRegistered() const 
{ 
    return isRegistered; 
}

// Setters
void Client::setNick(const std::string& nick)
{
    nickname = nick;
    hasNick = true;
    checkRegistration();
}

void Client::setUser(const std::string& user, const std::string& real)
{
    username = user;
    realname = real;
    hasUser = true;
    checkRegistration();
}

void Client::setPass(bool passed)
{
    hasPassed = passed;
    checkRegistration();
}

void Client::setRegistered(bool registered)
{
    isRegistered = registered;
}

void Client::setHostname(const std::string& host)
{
    hostname = host;
}

// Authentication check
void Client::checkRegistration()
{
    if (hasPassed && hasNick && hasUser && !isRegistered)
    {
        isRegistered = true;
    }
}
