#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "ft_irc.hpp"

class Server;

class Client
{
private:
    int         fd;
    int         clientSocket;
    std::string ip;
    int         port;

    std::string nickname;
    std::string username;
    std::string realname;
    
    bool        hasPassed;
    bool        hasNick;
    bool        hasUser;
    bool        isAuthed;
    bool        AuHasChecked;

public:
    Client(int fd, const std::string &ip, int port);
    Client(int fd);
    int getFd() const;

    void setNick(const std::string nick);
    void setUser(const std::string &user, const std::string &real);
    void setPass(bool ok);
    void setAuHChecked(bool ok);

    bool getHPass() const;
    bool getHNick() const;
    bool getHUser() const;
    bool getHAuthed() const;
    bool getAuHchecked() const;

    void authenticate();
    std::string getNick() const;
    std::string getUser() const;

    int     checkPassword(std::string data, Server const &server);
    int     checkNick(std::string data, Server const &server);
    int     checkUser(std::string data, Server const &server);

};

#endif