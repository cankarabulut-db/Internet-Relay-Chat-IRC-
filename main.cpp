#include "ft_irc.hpp"

std::string getAfterColon(const std::string& str)
{
    size_t colonPos = str.find(':');
    if (colonPos == std::string::npos)
    {
        return "";  // ':' bulunamazsa boş string döndür
    }
    return str.substr(colonPos + 1);
}

size_t findNthSpace(const std::string& str, int n) {
    size_t pos = -1;
    for (int i = 0; i < n; i++)
    {
        pos = str.find(' ', pos + 1);
        if (pos == std::string::npos) break;
    }
    return pos;
}

int     checkAuthentication(std::string data, Server &server, int client_socket)
{
    if (server.returnClient(client_socket).checkPassword(data, server) == 0)
        return (0);
    else if (server.returnClient(client_socket).checkNick(data, server) == 0)
        return (0);
    else if (server.returnClient(client_socket).checkUser(data, server) == 0)
        return (0);
    return (-1);
}

int main(int ac,char **av)
{
    if(ac != 3)
    {
        std::cout << "Usage : ./ircserver <port> <password>\n";
        return (1);
    }
    int port = atoi(av[1]);
    std::string password = (av[2]);
    if(port <= 0 || port > 65535)
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