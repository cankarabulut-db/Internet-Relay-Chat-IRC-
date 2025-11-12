#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <string>
#include <vector>

class Message
{
private:
    std::string prefix;
    std::string command;
    std::vector<std::string> params;
    std::string raw;

public:
    Message();
    Message(const std::string& rawMessage);
    
    bool parse(const std::string& rawMessage);
    
    const std::string& getPrefix() const;
    const std::string& getCommand() const;
    const std::vector<std::string>& getParams() const;
    size_t getParamCount() const;
    std::string getParam(size_t index) const;
    const std::string& getRaw() const;
    
    bool isValid() const;
};

// Helper functions
std::vector<std::string> splitMessages(const std::string& data);
std::string toUpper(const std::string& str);

#endif
