NAME = webserv
CC = c++
CCFLAGS =  -std=c++98 -g3 -fsanitize=address -Wall -Werror -Wextra
INC_LINK = -I./incs

SRCS_PATH = ./srcs

SRCS1 = get_next_line.cpp  get_next_line_utils.cpp \
	Request.cpp ParseRequest.cpp \
	rmain.cpp utils.cpp

SRCS2 = webserv.cpp config.cpp

SRCS_NAME = $(SRCS1) $(SRCS2)

SRCS = $(addprefix $(SRCS_PATH)/, $(SRCS_NAME))

OBJS_NAME = $(SRCS_NAME:.cpp=.obj)

OBJS_PATH = ./objs

OBJS = $(addprefix $(OBJS_PATH)/, $(OBJS_NAME))

# 리퀘스트 파트에서 테스트용으로 만든 변수들입니다

PSRCS = $(addprefix $(SRCS_PATH)/, $(SRCS1))

POBJS_NAME = $(SRCS1:.cpp=.obj)

POBJS = $(addprefix $(OBJS_PATH)/, $(POBJS_NAME))

#

all : $(NAME)

$(NAME) : $(OBJS)
	$(CC) $(CCFLAGS) -o $(NAME) $(OBJS)

rparse : $(POBJS)
	$(CC) $(CCFLAGS) -o $(NAME) $(POBJS)

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

.PHONY: all clean fclean