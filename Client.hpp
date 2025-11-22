#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>

class Client
{
private:
    int fd;
    std::string ip;
    int port;
    
    std::string nickname;
    std::string username;
    std::string realname;
    std::string hostname;
    
    bool hasPassed;
    bool hasNick;
    bool hasUser;
    bool isRegistered;

public:
    Client();
    Client(int fd, const std::string& ip, int port);
    
    int getFd() const;
    int getPort() const;
    std::string getNick() const;
    std::string getUser() const;
    std::string getReal() const;
    std::string getHostname() const;
    std::string getPrefix() const;
    
    bool getHasPassed() const;
    bool getHasNick() const;
    bool getHasUser() const;
    bool getIsRegistered() const;
    
    void setNick(const std::string& nick);
    void setUser(const std::string& user, const std::string& real);
    void setPass(bool passed);
    void setRegistered(bool registered);
    void setHostname(const std::string& host);
    
    void checkRegistration();
};

#endif