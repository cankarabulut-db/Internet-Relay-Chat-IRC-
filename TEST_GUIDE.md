# IRC Server Test Guide

## Starting the Server

```bash
./ircserv <port> <password>
```

Example:
```bash
./ircserv 6667 mypassword
```

## Testing with netcat (nc)

### Basic Authentication Test
```bash
nc localhost 6667
```

Then type:
```
PASS mypassword
NICK john
USER john 0 * :John Doe
```

You should receive welcome messages:
```
:irc.local 001 john :Welcome to the Internet Relay Network john!john@localhost
:irc.local 002 john :Your host is irc.local, running version 1.0
:irc.local 003 john :This server was created 2025
:irc.local 004 john :irc.local 1.0 o itklmo
```

### Channel Operations Test
```
JOIN #test
PRIVMSG #test :Hello everyone!
TOPIC #test :My channel topic
MODE #test +t
PART #test :Goodbye
QUIT
```

## Testing with irssi

```bash
irssi
```

In irssi:
```
/connect localhost 6667 mypassword
/nick john
/join #test
/msg #test Hello!
/topic #test New topic
/part #test
/quit
```

## Testing with Multiple Clients

### Terminal 1:
```bash
nc localhost 6667
PASS mypassword
NICK alice
USER alice 0 * :Alice User
JOIN #chat
```

### Terminal 2:
```bash
nc localhost 6667
PASS mypassword
NICK bob
USER bob 0 * :Bob User
JOIN #chat
PRIVMSG #chat :Hi Alice!
```

Alice should see:
```
:bob!bob@localhost PRIVMSG #chat :Hi Alice!
```

## Available Commands

### Authentication (must be done first):
- `PASS <password>` - Set password
- `NICK <nickname>` - Set nickname
- `USER <username> <mode> <unused> <realname>` - Set user info

### Channel Commands:
- `JOIN <#channel> [key]` - Join a channel
- `PART <#channel> [reason]` - Leave a channel
- `KICK <#channel> <user> [reason]` - Kick user (operator only)
- `INVITE <user> <#channel>` - Invite user
- `TOPIC <#channel> [topic]` - View or set topic
- `MODE <#channel> <modes> [params]` - Set channel modes

### Messaging:
- `PRIVMSG <target> <message>` - Send message to user or channel
- `QUIT [reason]` - Disconnect from server

### Channel Modes:
- `+i` - Invite-only
- `+t` - Topic restricted to operators
- `+k <key>` - Channel password
- `+o <user>` - Give operator privileges
- `+l <limit>` - User limit

## Expected Behavior

1. **Authentication**: PASS, NICK, USER must be sent in order before other commands
2. **Channels**: Channels start with `#`
3. **Channel Creator**: First user to JOIN a channel becomes operator
4. **Modes**: Only operators can change channel modes
5. **Private Messages**: Can send to users or channels
6. **Topic**: Can view anytime, but setting requires operator if `+t` is set

## Error Testing

### Wrong Password:
```
PASS wrongpass
```
Expected: `:irc.local 464 * :Password incorrect`

### No Nickname:
```
NICK
```
Expected: `:irc.local 431 * :No nickname given`

### Nickname in Use:
When another user has the nickname.
Expected: `:irc.local 433 * <nick> :Nickname is already in use`

### Channel Not Found:
```
JOIN #nonexistent
PART #nonexistent
```
Should create channel on JOIN, error on PART if not member.

### No Operator Rights:
As non-operator, try:
```
MODE #test +i
```
Expected: `:irc.local 482 <nick> #test :You're not channel operator`
