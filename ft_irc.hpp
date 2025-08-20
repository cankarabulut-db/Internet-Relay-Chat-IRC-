#ifndef FT_IRC
#define FT_IRC


#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <algorithm>
#include <fcntl.h>
#include <iostream>
#include <cstring>
#include <cerrno>
#include <vector>
#include <map>
#include <sys/epoll.h>
#include <unistd.h>
#include <cstring>
#include <stdio.h>

#include "Server.hpp"
#include "Client.hpp"

size_t	findNthSpace(const std::string& str, int n);
std::string getAfterColon(const std::string& str);
int     checkAuthentication(std::string data, Server &server, int client_server);
#endif