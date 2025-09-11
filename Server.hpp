#ifndef SERVER_HPP
#define SERVER_HPP

#include "ft_irc.hpp"
class Client;

class Server
{
private:
    std::string password;
    int port;
    int serverSocket;
    int epollFd;
    std::vector<int> clientSockets;
    std::map<int, Client> clientInfo;
    struct sockaddr_in serverAddr;
    const int MAX_EVENTS;
public:
    Server(int port, std::string password);
    void socketArrangement();
    int getSocket(){return serverSocket;};
    int getPollFd(){return epollFd;};
    void setNonBlock(int fd);
    void setSocketAndBind();
    void ePollThings();
    void Add_To_Epoll(int fd, uint32_t events);
    void mod_Epoll(int fd, uint32_t events);
    void acceptNewClient();
    void clientDataHandling(int client_Socket, Server &server); // fonksiyon değişti
    void dataSending(int cSock, const std::string &data);
    void Listen();
    void removeFromEpoll(int fd);
    void disconnect(int client);
    void run();



    Client &returnClient(int client_socket);
    std::string getPassword() const;
    ~Server()
    {
        if(serverSocket != -1)
            std::cerr << "socket closed.\n";
    };
    int CheckPRVMSG(const std::string &data, int clientSocket);
    bool hasNick(const std::string& nick) const;
    void sendLine(int fd, const std::string &line);

};

#endif