#include "ft_irc.hpp"

Server::Server(int port, std::string password) : port(port) , password(password){
    serverSocket = -1;
    epollFd = -1;
    memset(&serverAddr,0,sizeof(serverAddr));
};

void Server::setNonBlock()
{
    int flags = fcntl(serverSocket, F_GETFL, 0);
    if (flags == -1) {
         perror("fcntl(F_GETFL) failed");
         exit(1);
    }
    if (fcntl(serverSocket, F_SETFL, flags | O_NONBLOCK) == -1) {
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
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = serverSocket;     
    if (epoll_ctl(epollFd, EPOLL_CTL_ADD, serverSocket, &ev) == -1) {
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
	Listen();
	ePollThings(); 
}

void Server::run()
{
	struct epoll_event events[MAX_EVENTS];
	std::cout << "Server is starting." << std::endl;
	while(true)
	{
		std::cout << clientSockets.size() << std::endl;
		int numEvents = epoll_wait(epollFd,events,MAX_EVENTS,-1);
		  if (numEvents == -1) {
                if (errno == EINTR) {
                    continue; 
                }
                perror("epoll_wait() failed");
                break;
            }
		std::cout << "Epoll sonucu: " << numEvents << " event hazır" << std::endl;
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
    if(port <= 0 && port >= 65535)
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