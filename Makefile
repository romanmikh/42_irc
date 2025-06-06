# Colours
DEF_COLOR	=	\033[0;39m
GRAY		=	\033[0;90m
RED			=	\033[0;91m
GREEN		=	\033[0;92m
YEL			=	\033[0;93m
BLUE		=	\033[0;94m
MAGENTA		=	\033[0;95m
CYAN		=	\033[0;96m
WHITE		=	\033[0;97m

# Program file name
NAME	:= ircserv

# Compiler and compilation flags
CC		:= c++
CFLAGS	:= -Werror -Wextra -Wall -g3 -std=c++98

# Build files and directories
SRC_PATH 	= ./sources/
OBJ_PATH	= ./objects/
INC_PATH	= ./include/

SRCS = $(SRC_PATH)Channel.cpp \
		$(SRC_PATH)ChannelManager.cpp \
		$(SRC_PATH)Client.cpp \
		$(SRC_PATH)main.cpp \
		$(SRC_PATH)MsgHandler.cpp \
		$(SRC_PATH)QuoteBot.cpp \
		$(SRC_PATH)Server.cpp \
		$(SRC_PATH)utils/Error.cpp \
		$(SRC_PATH)utils/command.cpp \
		$(SRC_PATH)utils/Logger.cpp \
		$(SRC_PATH)utils/Utils.cpp

OBJS = $(SRCS:$(SRC_PATH)%.cpp=$(OBJ_PATH)%.o)
INC	= -I $(INC_PATH)

# Main rule
all: $(OBJ_PATH) $(NAME)

# Objects directory rule
$(OBJ_PATH):
	mkdir -p $(OBJ_PATH)

# Objects rule
$(OBJ_PATH)%.o: $(SRC_PATH)%.cpp
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@ $(INC)

# Project file rule
$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(INC)

# Clean up build files rule
clean:
	rm -rf $(OBJ_PATH)

# Remove program executable
fclean: clean
	rm -f $(NAME) valgrind_out.txt

# Clean + remove executable
re: fclean all

# Valgrind rule
valgrind: $(NAME)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --track-fds=yes --trace-children=yes --num-callers=20 --log-file=valgrind_out.txt ./ircserv 8080 abcd

.PHONY: all re clean fclean valgrind