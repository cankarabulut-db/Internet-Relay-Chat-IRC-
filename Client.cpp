#include "Client.hpp"

Client::Client(int fd): fd(fd), hasPassed(false), hasNick(false), hasUser(false), isAuthed(false) {}
Client::Client(int fd, const std::string &ip, int port): fd(fd)
, ip(ip), port(port), hasPassed(false), hasNick(false), hasUser(false), isAuthed(false) {}


int Client::getFd() const { return fd; }

void Client::setNick(const std::string nick) {
    this->nickname = nick;
    this->hasNick = true;
}

void Client::setUser(const std::string &user, const std::string &real) {
    this->username = user;
    this->realname = real;
    this->hasUser = true;
}

void Client::setPass(bool ok) {
    this->hasPassed = ok;
}

bool Client::getHPass() const { return hasPassed; }
bool Client::getHNick() const { return hasNick; }
bool Client::getHUser() const { return hasUser; }
bool Client::getHAuthed() const { return isAuthed; }

void Client::authenticate()
{
    std::cout << "p" << this->hasPassed << "n" << this->hasNick << "u" << this->hasUser << std::endl;
    if (this->hasPassed && this->hasNick && this->hasUser)
        this->isAuthed = true;
}

std::string Client::getNick() const { return nickname; }
std::string Client::getUser() const { return username; }

int     Client::checkNick(std::string data, Server const &server)
{
    (void)server; // bu kısma baka nick kontolu için
    const char* purpose = "NICK";
    if ((strcmp(data.substr(0, 4).c_str(), purpose) != 0))
    {
        // std::cout <<  "Invalid entry " << std::endl;
        return (-1);
    }
    this->setNick(data.substr(5, strlen(data.c_str()) - 5));
    this->authenticate();
    std::cout << "nick:" << this->hasNick <<this->getHAuthed() << "\n";
    return (0);
}

int     Client::checkUser(std::string data, Server const &server)
{
    (void)server;
    const char* purpose = "USER";
    if ((strcmp(data.substr(0, 4).c_str(), purpose) != 0) || (strlen(data.substr(5, 20).c_str()) > 20) || (strlen(data.substr(data.find(':'), 40).c_str()) > 40))
    {
        // std::cout <<  "Invalid entry " << std::endl;
        return (-1);
    }
    this->hasUser = true;
    this->setUser(data.substr(5, findNthSpace(data, 2) - 3), getAfterColon(data));
    this->authenticate();
    std::cout << "user:"<< this->hasUser << this->getHAuthed() << "\n";
    return (0);
}

int     Client::checkPassword(std::string data, Server const &server)
{
    const char* purpose = "PASS";
    if ((strcmp(data.substr(0, 4).c_str(), purpose) != 0) || (strcmp(server.getPassword().c_str(), data.substr(5, strlen(server.getPassword().c_str())).c_str()) != 0))
    {
        // std::cout <<  "Invalid entry " << std::endl;
        return (-1);
    }
    this->hasPassed = true;
    this->authenticate();
    std::cout <<"pass:" << this->hasPassed << this->getHAuthed() << "\n";
    return (0);
}