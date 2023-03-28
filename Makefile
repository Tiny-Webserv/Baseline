NAME = webserv
CC = c++
CCFLAGS = -Wall -Werror -Wextra -std=c++98

SRCS1 = get_next_line.c get_next_line_utils.c \
	Request.cpp

SRCS2 = webserv.cpp config.cpp

SRCS = $(SRCS1) $(SRCS2)

OBJS = $(SRCS:.cpp=.obj)

all : $(NAME)

$(NAME) : $(OBJS)
	$(CC) $(CCFLAGS) -o $(NAME) $(OBJS)

%.obj : %.cpp
	$(CC) $(CCFLAGS) -c $< -o $@

clean :
	rm -rf $(OBJS)

fclean : clean
	rm -rf $(NAME)
re :
	@make fclean
	@make all

.PHONY: all clean fclean