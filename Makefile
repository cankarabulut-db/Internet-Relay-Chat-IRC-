CXX = c++

CXXFLAGS = -Wall -Wextra -Werror -std=c++98

SRCS = main.cpp \
       Client.cpp \
       Server.cpp \
       Channel.cpp \
       Message.cpp \
       Replies.cpp \
       Commands.cpp

NAME = ircserv
CLIENT = ircclient

OBJS = $(SRCS:.cpp=.o)

all: $(NAME) $(CLIENT)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

$(CLIENT): interactive_client.cpp
	$(CXX) $(CXXFLAGS) interactive_client.cpp -o $(CLIENT)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME) $(CLIENT)

re: fclean all

.PHONY: all clean fclean re