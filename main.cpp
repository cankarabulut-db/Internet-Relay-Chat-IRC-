#include "ft_irc.hpp"
#include <cctype>

std::string trim(const std::string& str)
{
    std::string::size_type start = 0;
    std::string::size_type end = str.size();
    
    while (start < end && std::isspace(static_cast<unsigned char>(str[start])))
        ++start;
    
    while (end > start && std::isspace(static_cast<unsigned char>(str[end - 1])))
        --end;
    
    return str.substr(start, end - start);
}

bool isValidNickname(const std::string& nick)
{
    if (nick.empty() || nick.length() > 9)
        return false;
    
    // First character must be a letter
    if (!std::isalpha(static_cast<unsigned char>(nick[0])))
        return false;
    
    // Rest can be letters, digits, or special characters
    for (size_t i = 1; i < nick.length(); ++i)
    {
        char c = nick[i];
        if (!std::isalnum(static_cast<unsigned char>(c)) && 
            c != '-' && c != '_' && c != '[' && c != ']' && 
            c != '{' && c != '}' && c != '\\' && c != '|')
            return false;
    }
    
    return true;
}

bool isValidChannelName(const std::string& name)
{
    if (name.empty() || name.length() > 50)
        return false;
    
    // Must start with # or &
    if (name[0] != '#' && name[0] != '&')
        return false;
    
    // Check for invalid characters (space, comma, colon)
    for (size_t i = 1; i < name.length(); ++i)
    {
        char c = name[i];
        if (c == ' ' || c == ',' || c == ':' || c == 7)
            return false;
    }
    
    return true;
}

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        std::cout << "Usage: ./ircserv <port> <password>" << std::endl;
        return 1;
    }
    
    int port = atoi(argv[1]);
    std::string password = argv[2];
    
    if (port <= 0 || port > 65535)
    {
        std::cerr << "Error: Invalid port number" << std::endl;
        return 1;
    }
    
    if (password.empty())
    {
        std::cerr << "Error: Password cannot be empty" << std::endl;
        return 1;
    }
    
    std::cout << "================================" << std::endl;
    std::cout << "      IRC SERVER v1.0          " << std::endl;
    std::cout << "================================" << std::endl;
    std::cout << "Port: " << port << std::endl;
    std::cout << "Password: " << password << std::endl;
    std::cout << "================================" << std::endl;
    
    try
    {
        Server server(port, password);
        server.socketArrangement();
        server.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}