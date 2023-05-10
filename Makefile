NAME = webserv
CC = c++

#리터럴 string 무시 플래그 추가
CCFLAGS =  -std=c++98 -g3 -fsanitize=address -fsanitize=undefined -Wno-write-strings -Wall -Werror -Wextra

INC_LINK = -I./incs

SRCS_PATH = ./srcs

SRCS_NAME = webserv.cpp Config.cpp LocationBlock.cpp ServerBlock.cpp Socket.cpp EventLoop.cpp \
		ParseRequest.cpp Request.cpp utils.cpp get_next_line.cpp  get_next_line_utils.cpp \
		StateCode.cpp ServerFiles.cpp Response.cpp


SRCS = $(addprefix $(SRCS_PATH)/, $(SRCS_NAME))

OBJS_NAME = $(SRCS_NAME:.cpp=.obj)

OBJS_PATH = ./objs

OBJS = $(addprefix $(OBJS_PATH)/, $(OBJS_NAME))

all : $(NAME)

$(NAME) : $(OBJS)
	$(CC) $(CCFLAGS) -o $(NAME) $(OBJS)

$(OBJS_PATH)/%.obj : $(SRCS_PATH)/%.cpp
	mkdir -p ./objs
	$(CC) $(INC_LINK) $(CCFLAGS) -c $< -o $@

clean :
	rm -rf $(OBJS)

fclean : clean
	rm -rf $(NAME)
re :
	@make fclean
	@make all

.PHONY: all clean fclean re