#include "Message.hpp"
#include <algorithm>
#include <cctype>

Message::Message() {}

Message::Message(const std::string& rawMessage)
{
    parse(rawMessage);
}

bool Message::parse(const std::string& rawMessage)
{
    raw = rawMessage;
    prefix.clear();
    command.clear();
    params.clear();
    
    if (rawMessage.empty())
        return false;
    
    std::string line = rawMessage;
    
    // Remove trailing \r\n
    if (line.size() >= 2 && line.substr(line.size() - 2) == "\r\n")
        line = line.substr(0, line.size() - 2);
    else if (!line.empty() && line[line.size() - 1] == '\n')
        line = line.substr(0, line.size() - 1);
    
    if (line.empty())
        return false;
    
    size_t pos = 0;
    
    // Parse prefix (optional)
    if (line[0] == ':')
    {
        size_t spacePos = line.find(' ', 1);
        if (spacePos == std::string::npos)
            return false;
        
        prefix = line.substr(1, spacePos - 1);
        pos = spacePos + 1;
    }
    
    // Skip spaces
    while (pos < line.size() && line[pos] == ' ')
        ++pos;
    
    if (pos >= line.size())
        return false;
    
    // Parse command
    size_t cmdEnd = pos;
    while (cmdEnd < line.size() && line[cmdEnd] != ' ')
        ++cmdEnd;
    
    command = line.substr(pos, cmdEnd - pos);
    pos = cmdEnd;
    
    // Parse parameters
    while (pos < line.size())
    {
        // Skip spaces
        while (pos < line.size() && line[pos] == ' ')
            ++pos;
        
        if (pos >= line.size())
            break;
        
        // Trailing parameter (starts with ':')
        if (line[pos] == ':')
        {
            params.push_back(line.substr(pos + 1));
            break;
        }
        
        // Normal parameter
        size_t paramEnd = pos;
        while (paramEnd < line.size() && line[paramEnd] != ' ')
            ++paramEnd;
        
        params.push_back(line.substr(pos, paramEnd - pos));
        pos = paramEnd;
    }
    
    return !command.empty();
}

const std::string& Message::getPrefix() const
{
    return prefix;
}

const std::string& Message::getCommand() const
{
    return command;
}

const std::vector<std::string>& Message::getParams() const
{
    return params;
}

size_t Message::getParamCount() const
{
    return params.size();
}

std::string Message::getParam(size_t index) const
{
    if (index < params.size())
        return params[index];
    return "";
}

const std::string& Message::getRaw() const
{
    return raw;
}

bool Message::isValid() const
{
    return !command.empty();
}

// Helper functions
std::vector<std::string> splitMessages(const std::string& data)
{
    std::vector<std::string> messages;
    std::string buffer = data;
    size_t pos = 0;
    
    while ((pos = buffer.find("\r\n")) != std::string::npos)
    {
        std::string msg = buffer.substr(0, pos + 2);
        if (!msg.empty())
            messages.push_back(msg);
        buffer = buffer.substr(pos + 2);
    }
    
    return messages;
}

std::string toUpper(const std::string& str)
{
    std::string result = str;
    for (size_t i = 0; i < result.size(); ++i)
        result[i] = std::toupper(static_cast<unsigned char>(result[i]));
    return result;
}
