# ft_irc - Internet Relay Chat Server

Implementation of an IRC server in C++98 following RFC 1459 and RFC 2812 specifications, supporting multiple clients, channels, and essential IRC commands.

## Features

✅ **Authentication System**
- Password protection (PASS command)
- Nickname management (NICK command)
- User registration (USER command)

✅ **Channel Management**
- Create and join channels (JOIN command)
- Leave channels (PART command)
- Channel operators with special privileges
- Invite-only channels (+i mode)
- Channel topics with restrictions (+t mode)
- Channel passwords (+k mode)
- User limits (+l mode)
- Operator privileges (+o mode)

✅ **Messaging**
- Private messages between users (PRIVMSG)
- Channel broadcasts
- Server notifications

✅ **Moderation**
- KICK users from channels (operator only)
- INVITE users to channels
- Set and modify TOPIC (with restrictions)
- Channel MODE management

✅ **IRC Protocol Compliance**
- Proper message parsing (prefix, command, parameters)
- Standard IRC numeric replies (001-502)
- Support for both \r\n and \n line endings
- Non-blocking I/O with epoll

## Build

```bash
make
```

## Usage

```bash
./ircserv <port> <password>
```

Example:
```bash
./ircserv 6667 mypassword
```

## Testing

### Quick Test with netcat:
```bash
nc localhost 6667
PASS mypassword
NICK alice
USER alice 0 * :Alice User
JOIN #general
PRIVMSG #general :Hello World!
QUIT
```

### Test with irssi:
```bash
irssi
/connect localhost 6667 mypassword
/nick alice
/join #general
```

For comprehensive testing guide, see [TEST_GUIDE.md](TEST_GUIDE.md)

## Supported Commands

| Command | Description | Example |
|---------|-------------|---------|
| PASS | Set connection password | `PASS mypassword` |
| NICK | Set or change nickname | `NICK alice` |
| USER | Set user information | `USER alice 0 * :Alice User` |
| JOIN | Join a channel | `JOIN #general` |
| PART | Leave a channel | `PART #general :Goodbye` |
| PRIVMSG | Send message | `PRIVMSG #general :Hello!` |
| KICK | Kick user (op only) | `KICK #general bob :Spam` |
| INVITE | Invite user | `INVITE bob #general` |
| TOPIC | View/set topic | `TOPIC #general :Welcome!` |
| MODE | Set channel modes | `MODE #general +it` |
| QUIT | Disconnect | `QUIT :Leaving` |

## Channel Modes

| Mode | Description | Requires Parameter |
|------|-------------|-------------------|
| +i | Invite-only | No |
| +t | Topic restricted to operators | No |
| +k | Channel password | Yes (key) |
| +o | Operator privileges | Yes (nickname) |
| +l | User limit | Yes (number) |

## Architecture

- **Server.cpp**: Main server logic, epoll-based event handling
- **Client.cpp**: Client state management and authentication
- **Channel.cpp**: Channel management, members, operators, modes
- **Message.cpp**: IRC protocol message parsing
- **Commands.cpp**: All IRC command handlers
- **Replies.cpp**: Standard IRC numeric replies

## Requirements

- C++98 compliant compiler
- Linux (epoll support required)
- Make

## Authors

Developed as part of the 42 School curriculum.

## License

This is an educational project.
