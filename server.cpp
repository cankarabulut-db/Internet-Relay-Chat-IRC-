#include "ft_irc.hpp"

Server::Server(int port, std::string password) : port(port) , password(password){
    serverSocket = -1;
    epollFd = -1;
    memset(&serverAddr,0,sizeof(serverAddr));
};

void Server::setNonBlock(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    // if (flags == -1) {
    //      perror("fcntl(F_GETFL) failed");
    //      exit(1);
    // }
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
         perror("fcntl(F_SETFL) failed");
        exit(1);
    }
}

void Server::setSocketAndBind()
{
    int opt = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        std::cerr << "Warning: setsockopt(SO_REUSEADDR) failed" << std::endl;
        exit(1);
    }
        serverAddr.sin_family = AF_INET;           
        serverAddr.sin_addr.s_addr = INADDR_ANY;   
        serverAddr.sin_port = htons(port);         
        if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
            std::cerr << "Error: bind() failed: " << strerror(errno) << std::endl;
            close(serverSocket);
            exit(1);
        }        
}

void Server::Listen()
{
	if (listen(serverSocket, 10) == -1) {
    	std::cerr << "Error: listen() failed: " << strerror(errno) << std::endl;
    	close(serverSocket);
        exit(1);
    }
    std::cout << "Waiting for connections..." << std::endl;
}

void Server::ePollThings()
{
	epollFd = epoll_create1(EPOLL_CLOEXEC);
    if (epollFd == -1) {
        perror("epoll_create1() failed");
        close(serverSocket);
        exit(1);
    }
    Add_To_Epoll(serverSocket,EPOLLIN);
}

void Server::Add_To_Epoll(int fd, uint32_t events) {
        struct epoll_event ev;
        ev.events = events;
        ev.data.fd = fd;
        
        if (epoll_ctl(epollFd, EPOLL_CTL_ADD, fd, &ev) == -1) {
            perror("epoll_ctl(ADD) failed");
            exit(1);
        }
    }

void Server::removeFromEpoll(int fd) {
        if (epoll_ctl(epollFd, EPOLL_CTL_DEL, fd, NULL) == -1) {
            perror("epoll_ctl(DEL) failed");
        }
}


void Server::socketArrangement()
{
    this->serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(serverSocket == -1)
    {
        std::cerr << "Error : socket could'nt created." << std::endl;
        exit(1);
    }
    setSocketAndBind();
    setNonBlock(serverSocket);
	Listen();
	ePollThings(); 
}

void Server::mod_Epoll(int fd, uint32_t events) {
    struct epoll_event ev;
    ev.events = events;
    ev.data.fd = fd;
        
    if (epoll_ctl(epollFd, EPOLL_CTL_MOD, fd, &ev) == -1)
        perror("epoll_ctl(MOD) failed");
}

void Server::acceptNewClient()
{
    while(true)
    {
        struct sockaddr_in clientAddress;
        socklen_t clientLength = sizeof(clientAddress);

        int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress,&clientLength);
        // client socket error handle -1
        if (clientSocket == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                std::cout << "Bekleyen bağlantı yok (non-blocking)" << std::endl;
                break;
            } else {
                perror("accept() failed");
                break;
            }
        }
        
        char clientIPaddr[INET_ADDRSTRLEN]; // AFINET İP4 AFINET6 İP6
        inet_ntop(AF_INET,&clientAddress.sin_addr, clientIPaddr, INET_ADDRSTRLEN);
        int clientPort = ntohs(clientAddress.sin_port);

        std::cout << "new client accepted.\n";

        setNonBlock(clientSocket);
        Add_To_Epoll(clientSocket, EPOLLIN | EPOLLET);
        clientSockets.push_back(clientSocket);
        clientInfo[clientSocket] = std::string(clientIPaddr) + ":" + std::to_string(clientPort); 
        std::cout << "✅ client added\n";
    }
}

void Server::dataSending(int cSock, const std::string &data)
{
    ssize_t totalSent = 0;
    ssize_t dataSize = data.length();

    while(dataSize > totalSent)
    {
        ssize_t sent = send(cSock, data.c_str() + totalSent, dataSize - totalSent, MSG_NOSIGNAL);
        if(sent == -1)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    // Socket buffer dolu, epoll'da yazma için bekle
                    mod_Epoll(cSock, EPOLLIN | EPOLLOUT | EPOLLET);
                    break;
                } else {
                    perror("send() failed");
                    std::cout << "disconnect\n";
                    break;
                }
        }
        else
            totalSent += sent;
    }
    if(totalSent == dataSize)
        std::cout << "veri gitti\n";
}

void Server::clientData(int client_Socket)
{
    std::cout << "Bu client : " << client_Socket << "veri gönderdi." << std::endl;
    char buffer[4096];
    std::string data;

    while(true)
    {
        memset(buffer, 0, sizeof(buffer));
        int bytesRead = recv(client_Socket,buffer, sizeof(buffer) - 1, 0);
        if(bytesRead > 0)
        {
            buffer[bytesRead] = 0;
            data += buffer;
            std::cout << "Okunan (" << bytesRead << " bytes): [" << buffer << "]" << std::endl;
        }
        else if(bytesRead == 0)
        {
            std::cout << "disconnect" << std::endl;
            return ;
        }
        else{
             if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    // Tüm veri okundu
                    break;
                } else {
                    perror("recv() failed");
                    std::cout << "disconnect " << std::endl;
                    return;
                }
        }
        if(!data.empty())
        {
            std::cout << "data : " << data << std::endl;

            std::string deneme = "EPOLL-ECHO " + data;
            dataSending(client_Socket, deneme);
        }
    }
}

void Server::run()
{
	int i;

	i = 0;
	struct epoll_event events[MAX_EVENTS];
	std::cout << "Server is starting." << std::endl;
	while(true)
	{
		int numEvents = epoll_wait(epollFd,events,MAX_EVENTS,-1);
		  if (numEvents == -1) {
                if (errno == EINTR) {
                    continue; 
                }
                perror("epoll_wait() failed");
                break;
            }
		for (int i = 0; i < numEvents; i++)
		{
			int eventFd = events[i].data.fd;
			uint32_t eventFlags = events[i].events;
			if(eventFd == serverSocket) // YENİ TANIMLANMIŞ CLİENT
			{
				if(eventFlags & EPOLLIN)
                {
					std::cout << "New Client!" << std::endl;
                    acceptNewClient();
                }
			}
			else 
				{ 
					if(eventFlags & (EPOLLERR | EPOLLHUP)) // CLİENT ERROR
					{
						std::cout << "disconnected." << std::endl;
					}
                    else if(eventFlags & EPOLLIN) // TANIMLI CLİENT INPUT
                    {
                        std::cout << "input alındı." << std::endl;
                        clientData(eventFd);
                    }
                    else if(eventFlags & EPOLLOUT)
                        std::cout << "epollout\n";
				}
			
		}
	}
}

int main(int ac,char **av)
{
    int port = atoi(av[1]);
    std::string password = (av[2]);
    if(ac != 3)
    {
        std::cout << "Usage : ./ircserver <port> <password>\n";
        return (1);
    }
    if(port <= 0 && port > 65535)
    {
        std::cerr << "Wrong port address.\n";
        return (1); 
    }
	std::cout << "<===============================>\n";
	std::cout << "<==>        IRC SERVER       <==>\n";
	std::cout << "<===============================>\n";
	std::cout << "<==>       PORT : " << port << "       <==>\n" ;
	std::cout << "<===============================>\n";
	std::cout << "* Password : " << password << std::endl;
   try 
   {
    	Server server(port, password);
     
    	server.socketArrangement();
        server.run();
        std::cin.get();
        
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
    return (0);
}