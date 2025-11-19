#ifndef SERVER_HPP
#define SERVER_HPP

#include "ft_irc.hpp"

class Client;
class Channel;
class Message;

class Server
{
private:
    std::string password;
    int port;
    int serverSocket;
    int epollFd;
    std::vector<int> clientSockets;
    std::map<int, Client> clientInfo;
    std::map<std::string, Channel*> channels;
    std::map<int, std::string> clientBuffers;
    struct sockaddr_in serverAddr;
    const int MAX_EVENTS;

public:
    Server(int port, std::string password);
    ~Server();
    
    void socketArrangement();
    void setNonBlock(int fd);
    void setSocketAndBind();
    void ePollThings();
    void Listen();
    
    void addToEpoll(int fd, uint32_t events);
    void modEpoll(int fd, uint32_t events);
    void removeFromEpoll(int fd);
    
    void acceptNewClient();
    void handleClientData(int clientSocket);
    void disconnect(int clientSocket);
    
    void processMessage(int clientSocket, const Message& msg);
    void sendMessage(int fd, const std::string &data);
    
    void handleCap(int fd, const Message& msg);
    void handlePass(int fd, const Message& msg);
    void handleNick(int fd, const Message& msg);
    void handleUser(int fd, const Message& msg);
    void handleJoin(int fd, const Message& msg);
    void handlePart(int fd, const Message& msg);
    void handlePrivmsg(int fd, const Message& msg);
    void handleKick(int fd, const Message& msg);
    void handleInvite(int fd, const Message& msg);
    void handleTopic(int fd, const Message& msg);
    void handleMode(int fd, const Message& msg);
    void handleQuit(int fd, const Message& msg);
    void handleWho(int fd, const Message& msg);
    void handleHelp(int fd, const Message& msg);
    
    Client& getClient(int fd);
    const Client& getClient(int fd) const;
    bool hasNick(const std::string& nick) const;
    int getFdByNick(const std::string& nick) const;
    std::string getNickByFd(int fd) const;
    std::string getPassword() const;
    
    Channel* getChannel(const std::string& name);
    Channel* createChannel(const std::string& name);
    void deleteChannel(const std::string& name);
    bool channelExists(const std::string& name) const;
    void removeClientFromAllChannels(int fd);
    
    void broadcastToChannel(const std::string& channelName, const std::string& message, int excludeFd);
    
    void run();
    
    int getSocket() const { return serverSocket; }
    int getPollFd() const { return epollFd; }
};

#endif