#ifndef FT_IRC_HPP
#define FT_IRC_HPP

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <fcntl.h>

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <cstring>
#include <cerrno>
#include <sstream>
#include <cstdlib>
#include <cstdio>

#include "Client.hpp"
#include "Server.hpp"
#include "Channel.hpp"
#include "Message.hpp"
#include "Replies.hpp"

std::string trim(const std::string& str);
bool isValidNickname(const std::string& nick);
bool isValidChannelName(const std::string& name);

#endif