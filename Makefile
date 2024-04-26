
CC = c++
CFLAGS = -Wall -Wextra -Werror #-fsanitize=address -g3
NAME = ircserv
SRCS = $(shell find . -type f -name "*.cpp")
OBJS = $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJS)

fclean: clean
	rm -rf $(NAME)

re: fclean all

push:
	git add .
	git commit -m "update"
	git push