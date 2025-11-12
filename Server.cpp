#include "ft_irc.hpp"
#include <iostream>

Server::Server(int port, std::string password) 
    : password(password), port(port), serverSocket(-1), epollFd(-1), MAX_EVENTS(21)
{
    memset(&serverAddr, 0, sizeof(serverAddr));
}

Server::~Server()
{
    for (std::map<std::string, Channel*>::iterator it = channels.begin(); 
         it != channels.end(); ++it)
    {
        delete it->second;
    }
    channels.clear();
    
    for (size_t i = 0; i < clientSockets.size(); ++i)
    {
        close(clientSockets[i]);
    }
    
    if (epollFd != -1)
        close(epollFd);
    
    if (serverSocket != -1)
    {
        close(serverSocket);
        std::cout << "Server socket closed." << std::endl;
    }
}

void Server::setNonBlock(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1 || fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
    {
        perror("fcntl(F_SETFL) failed");
        exit(1);
    }
}

void Server::setSocketAndBind()
{
    int opt = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
    {
        std::cerr << "Warning: setsockopt(SO_REUSEADDR) failed" << std::endl;
        exit(1);
    }
    
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);
    
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1)
    {
        std::cerr << "Error: bind() failed: " << strerror(errno) << std::endl;
        close(serverSocket);
        exit(1);
    }
}

void Server::Listen()
{
    if (listen(serverSocket, 10) == -1)
    {
        std::cerr << "Error: listen() failed: " << strerror(errno) << std::endl;
        close(serverSocket);
        exit(1);
    }
    std::cout << "Server listening on port " << port << "..." << std::endl;
}

void Server::ePollThings()
{
    epollFd = epoll_create1(EPOLL_CLOEXEC);
    if (epollFd == -1)
    {
        perror("epoll_create1() failed");
        close(serverSocket);
        exit(1);
    }
    addToEpoll(serverSocket, EPOLLIN);
}

void Server::socketArrangement()
{
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1)
    {
        std::cerr << "Error: socket couldn't be created." << std::endl;
        exit(1);
    }
    
    setSocketAndBind();
    setNonBlock(serverSocket);
    Listen();
    ePollThings();
}

void Server::addToEpoll(int fd, uint32_t events)
{
    struct epoll_event ev;
    ev.events = events;
    ev.data.fd = fd;
    
    if (epoll_ctl(epollFd, EPOLL_CTL_ADD, fd, &ev) == -1)
    {
        perror("epoll_ctl(ADD) failed");
        exit(1);
    }
}

void Server::removeFromEpoll(int fd)
{
    if (epoll_ctl(epollFd, EPOLL_CTL_DEL, fd, NULL) == -1)
    {
        perror("epoll_ctl(DEL) failed");
    }
}

void Server::modEpoll(int fd, uint32_t events)
{
    struct epoll_event ev;
    ev.events = events;
    ev.data.fd = fd;
    
    if (epoll_ctl(epollFd, EPOLL_CTL_MOD, fd, &ev) == -1)
        perror("epoll_ctl(MOD) failed");
}

void Server::acceptNewClient()
{
    while (true)
    {
        struct sockaddr_in clientAddress;
        socklen_t clientLength = sizeof(clientAddress);
        
        int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientLength);
        
        if (clientSocket == -1)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                break;
            else
            {
                perror("accept() failed");
                break;
            }
        }
        
        char clientIPaddr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &clientAddress.sin_addr, clientIPaddr, INET_ADDRSTRLEN);
        int clientPort = ntohs(clientAddress.sin_port);
        
        std::cout << "New client connected: " << clientSocket << std::endl;
        
        setNonBlock(clientSocket);
        addToEpoll(clientSocket, EPOLLIN | EPOLLET);
        clientSockets.push_back(clientSocket);
        clientInfo.insert(std::make_pair(clientSocket, Client(clientSocket, clientIPaddr, clientPort)));
        clientBuffers[clientSocket] = "";
    }
}

void Server::disconnect(int clientSocket)
{
    std::cout << "Disconnecting client: " << clientSocket << std::endl;
    
    removeClientFromAllChannels(clientSocket);
    removeFromEpoll(clientSocket);
    close(clientSocket);
    
    std::vector<int>::iterator it = std::find(clientSockets.begin(), clientSockets.end(), clientSocket);
    if (it != clientSockets.end())
        clientSockets.erase(it);
    
    clientInfo.erase(clientSocket);
    clientBuffers.erase(clientSocket);
    
    std::cout << "Client " << clientSocket << " disconnected." << std::endl;
}

void Server::handleClientData(int clientSocket)
{
    char buffer[4096];
    
    while (true)
    {
        memset(buffer, 0, sizeof(buffer));
        int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        
        if (bytesRead > 0)
        {
            buffer[bytesRead] = '\0';
            clientBuffers[clientSocket] += buffer;
        }
        else if (bytesRead == 0)
        {
            disconnect(clientSocket);
            return;
        }
        else
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                break;
            else
            {
                perror("recv() failed");
                disconnect(clientSocket);
                return;
            }
        }
    }
    
    // Process complete messages (handle both \r\n and \n)
    std::string& buffer_ref = clientBuffers[clientSocket];
    size_t pos;
    
    while ((pos = buffer_ref.find('\n')) != std::string::npos)
    {
        std::string line = buffer_ref.substr(0, pos);
        
        // Remove \r if present
        if (!line.empty() && line[line.length() - 1] == '\r')
            line = line.substr(0, line.length() - 1);
        
        buffer_ref = buffer_ref.substr(pos + 1);
        
        if (!line.empty())
        {
            Message msg(line + "\r\n");
            if (msg.isValid())
            {
                processMessage(clientSocket, msg);
            }
        }
    }
}

void Server::sendMessage(int fd, const std::string& data)
{
    std::string msg = data;
    if (msg.size() < 2 || msg.substr(msg.size() - 2) != "\r\n")
        msg += "\r\n";
    
    ssize_t totalSent = 0;
    ssize_t dataSize = msg.length();
    
    while (dataSize > totalSent)
    {
        ssize_t sent = send(fd, msg.c_str() + totalSent, dataSize - totalSent, MSG_NOSIGNAL);
        
        if (sent == -1)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                modEpoll(fd, EPOLLIN | EPOLLOUT | EPOLLET);
                break;
            }
            else
            {
                perror("send() failed");
                disconnect(fd);
                break;
            }
        }
        else
            totalSent += sent;
    }
}

Client& Server::getClient(int fd)
{
    std::map<int, Client>::iterator it = clientInfo.find(fd);
    if (it == clientInfo.end())
        throw std::runtime_error("Client not found");
    return it->second;
}

const Client& Server::getClient(int fd) const
{
    std::map<int, Client>::const_iterator it = clientInfo.find(fd);
    if (it == clientInfo.end())
        throw std::runtime_error("Client not found");
    return it->second;
}

bool Server::hasNick(const std::string& nick) const
{
    for (std::map<int, Client>::const_iterator it = clientInfo.begin();
         it != clientInfo.end(); ++it)
    {
        if (it->second.getNick() == nick)
            return true;
    }
    return false;
}

int Server::getFdByNick(const std::string& nick) const
{
    for (std::map<int, Client>::const_iterator it = clientInfo.begin();
         it != clientInfo.end(); ++it)
    {
        if (it->second.getNick() == nick)
            return it->first;
    }
    return -1;
}

std::string Server::getNickByFd(int fd) const
{
    std::map<int, Client>::const_iterator it = clientInfo.find(fd);
    if (it != clientInfo.end())
        return it->second.getNick();
    return "";
}

std::string Server::getPassword() const
{
    return password;
}

Channel* Server::getChannel(const std::string& name)
{
    std::map<std::string, Channel*>::iterator it = channels.find(name);
    if (it != channels.end())
        return it->second;
    return NULL;
}

Channel* Server::createChannel(const std::string& name)
{
    if (channelExists(name))
        return getChannel(name);
    
    Channel* newChannel = new Channel(name);
    channels[name] = newChannel;
    return newChannel;
}

void Server::deleteChannel(const std::string& name)
{
    std::map<std::string, Channel*>::iterator it = channels.find(name);
    if (it != channels.end())
    {
        delete it->second;
        channels.erase(it);
    }
}

bool Server::channelExists(const std::string& name) const
{
    return channels.find(name) != channels.end();
}

void Server::removeClientFromAllChannels(int fd)
{
    std::vector<std::string> toDelete;
    
    for (std::map<std::string, Channel*>::iterator it = channels.begin();
         it != channels.end(); ++it)
    {
        if (it->second->isMember(fd))
        {
            it->second->removeMember(fd);
            
            if (clientInfo.find(fd) != clientInfo.end())
            {
                std::string partMsg = ":" + getClient(fd).getPrefix() + " PART " + it->first + " :Client disconnected\r\n";
                broadcastToChannel(it->first, partMsg, -1);
            }
            
            if (it->second->getMemberCount() == 0)
                toDelete.push_back(it->first);
        }
    }
    
    for (size_t i = 0; i < toDelete.size(); ++i)
        deleteChannel(toDelete[i]);
}

void Server::broadcastToChannel(const std::string& channelName, const std::string& message, int excludeFd)
{
    Channel* channel = getChannel(channelName);
    if (!channel)
        return;
    
    const std::set<int>& members = channel->getMembers();
    for (std::set<int>::const_iterator it = members.begin(); it != members.end(); ++it)
    {
        if (*it != excludeFd)
            sendMessage(*it, message);
    }
}

void Server::run()
{
    struct epoll_event events[MAX_EVENTS];
    std::cout << "Server is running..." << std::endl;
    
    while (true)
    {
        int numEvents = epoll_wait(epollFd, events, MAX_EVENTS, -1);
        
        if (numEvents == -1)
        {
            if (errno == EINTR)
                continue;
            perror("epoll_wait() failed");
            break;
        }
        
        for (int i = 0; i < numEvents; ++i)
        {
            int eventFd = events[i].data.fd;
            uint32_t eventFlags = events[i].events;
            
            if (eventFd == serverSocket)
            {
                if (eventFlags & EPOLLIN)
                    acceptNewClient();
            }
            else
            {
                if (eventFlags & (EPOLLERR | EPOLLHUP))
                {
                    disconnect(eventFd);
                }
                else if (eventFlags & EPOLLIN)
                {
                    handleClientData(eventFd);
                }
            }
        }
    }
}
