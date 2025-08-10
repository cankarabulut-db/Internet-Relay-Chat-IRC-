#ifndef FT_IRC
#define FT_IRC


#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <cstring>
#include <cerrno>
#include <vector>
#include <map>
#include <sys/epoll.h>


class Server {
private:
    std::string password;
    int port;
    int serverSocket;
    int epollFd;
    std::vector<int> clientSockets;
    std::map<int, std::string> clientInfo;
    struct sockaddr_in serverAddr;
    const int MAX_EVENTS = 21;
public:
    Server(int port, std::string password);
    void socketArrangement();
    int getSocket(){return serverSocket;};
    int getPollFd(){return epollFd;};
    void setNonBlock();
    void setSocketAndBind();
    void ePollThings();
    void Listen();
    void removeFromEpoll(int fd);
    void run(); 
    ~Server()
    {
        if(serverSocket != -1)
            std::cerr << "socket closed.\n";
    };
};

#endif