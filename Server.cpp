#include "ft_irc.hpp"


// IRC için ince sarmalayıcı
void Server::sendLine(int fd, const std::string &line)
{
    std::string out = line;
    if (out.size() < 2 || out.substr(out.size()-2) != "\r\n")
        out += "\r\n";               // IRC satır sonu garanti
    dataSending(fd, out);            // senin mevcut fonksiyon
}

int Server::fdByNick(const std::string &nick) const
{
    if (nick.empty())
        return -1;

    for (std::map<int, Client>::const_iterator it = clientInfo.begin();
         it != clientInfo.end(); ++it) {
        if (it->second.getNick() == nick)
            return it->first; // fd
    }
    return -1; // bulunamadı
}

std::string Server::nickByFd(int fd) const
{
    std::map<int, Client>::const_iterator it = clientInfo.find(fd);
    if (it != clientInfo.end())
        return it->second.getNick();
    else
        return "";
}


int Server::CheckPRVMSG(const std::string &data, int clientSocket)
{
    std::string s = trim(data);
    if (s.size() < 9) 
        return -1;

    if (s.compare(0, 8, "PRIVMSG ") != 0) 
        return -1;

    std::string rest = s.substr(8);
    std::string::size_type sp = rest.find(' ');
    if (sp == std::string::npos || sp == 0) 
        return -1;

    std::string target = rest.substr(0, sp);

    std::string::size_type i = sp + 1;
    while (i < rest.size() && (rest[i] == ' ' || rest[i] == '\t'))
        ++i;
    if (i >= rest.size()) 
        return -1;

    std::string tail = rest.substr(i);
    if (tail.empty() || tail[0] != ':') 
        return -1;

    std::string msg = trim(tail.substr(1));
    if (msg.empty()) 
        return -1;

    if (!this->hasNick(target))
    {
        sendLine(this->returnClient(clientSocket).getFd(), ": irc.local  401 " + this->returnClient(clientSocket).getNick() + " " + target + " :No such nick/channel");
        return -2; // hedef yok
    }
    int toFd = this->fdByNick(target);
    if (toFd < 0) {
        // hedef yok
        return -2;
    }

    // 5) Gönderenin nick’ini doğrula (opsiyonel ama sağlamlık için güzel)
    std::string fromNick = this->nickByFd(clientSocket);
    if (fromNick.empty()) {
        // Gönderen kayıtlı değil veya nick set edilmemiş → parse fail say
        return -1;
    }
    return 0; // her şey doğru
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


Client& Server::returnClient(int client_socket)
{
    std::map<int, Client>::iterator it = clientInfo.find(client_socket);
    if (it == clientInfo.end()) {
        throw std::runtime_error("Client not found");
    }
    return it->second;
}

std::string Server::getPassword() const
{
    return this->password;
}

Server::Server(int port, std::string password) :password(password) ,port(port), MAX_EVENTS(21)
{
    serverSocket = -1;
    epollFd = -1;
    memset(&serverAddr,0,sizeof(serverAddr));
};

void Server::setNonBlock(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
    {
         perror("fcntl(F_SETFL) failed");
        exit(1);
    }
}
void Server::disconnect(int clientSocket) 
{
    std::cout << "İstemci " << clientSocket << " çıkarılıyor..." << std::endl;
    
    // Epoll'dan çıkar
    removeFromEpoll(clientSocket);
    
    // Socket'i kapat
    close(clientSocket);
    
    // Vector'dan sil - C++98 uyumlu iterator
    std::vector<int>::iterator it = std::find(clientSockets.begin(), clientSockets.end(), clientSocket);
    if (it != clientSockets.end()) {
        clientSockets.erase(it);
    }
    
    // Map'ten sil
    clientInfo.erase(clientSocket);
    
    std::cout << "✓ Client : " << clientSocket << " deleted."<< std::endl;
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
    std::cout << this->serverSocket << std::endl;
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
        clientInfo.insert(std::make_pair(clientSocket, Client(clientSocket, clientIPaddr, clientPort)));
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
                    mod_Epoll(cSock, EPOLLIN | EPOLLOUT | EPOLLET);
                    break;
                } else {
                    perror("send() failed");
                    disconnect(cSock);
                    break;
                }
        }
        else
            totalSent += sent;
    }
    if(totalSent == dataSize)
        std::cout << "Data sent.\n";
}

void Server::clientDataHandling(int client_Socket, Server &server)
{
    char buffer[4096];
    std::string data;
    (void)server;
    while(true)
    {
        memset(buffer, 0, sizeof(buffer));
        int bytesRead = recv(client_Socket,buffer, sizeof(buffer) - 1, 0);
        if(bytesRead > 0)
        {
            buffer[bytesRead] = 0;
            data += buffer;
          //  std::cout << data << std::endl;
        }
        else if(bytesRead == 0)
        {
            disconnect(client_Socket);
            return ;
        }
        else{
             if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    break;
                } else {
                    perror("recv() failed");
                    disconnect(client_Socket);
                    return;
                }
        }
        if(!data.empty())
        {
            if (checkAuthentication(data, server, client_Socket) == -1)
            {
                ////////////////////////////// EXIT PART ///////////////////////////BAZEN BURAYA 2 KERE GİRİYOR NEDENİNİ ANLAMADIM ŞEKİLDE BUNA SONRA BAK
                perror("Authentication failed");
                //disconnect(client_Socket);
            }
            else
            {
                if (server.returnClient(client_Socket).getHAuthed() && server.returnClient(client_Socket).getAuHchecked())
                {
                    std::cout << "veri : " << data << std::endl;
                     //PARSE KISIM
                    std::string deneme = "MESAJ :  " + data;
                    std::string cong = server.returnClient(client_Socket).getNick() + "congrates now you are with us";
                    dataSending(client_Socket, cong);
                    server.returnClient(client_Socket).setAuHChecked(false);
                }
                if (CheckPRVMSG(data, client_Socket))
                    dataSending(client_Socket, "401 :No such nick");
            }
        }
    }
}
void Server::run()
{
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
			if(eventFd == serverSocket)
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
                        disconnect(eventFd);
					}
                    else if(eventFlags & EPOLLIN) // CLİENT TANIMLANDIKTAN SONRA GİRİLEN İNPUTLAR
                    {
                        // PARSİNG KISMI
                        clientDataHandling(eventFd, *this);
                    }
                    else if(eventFlags & EPOLLOUT)
                        std::cout << "epollout\n";
				}
			
		}
	}
}
