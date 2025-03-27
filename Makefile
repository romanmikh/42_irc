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
CFLAGS	:= -Werror -Wextra -Wall -std=c++98

# Build files and directories
SRC_PATH 	= ./sources/
OBJ_PATH	:= ./objects/
INC_PATH	= ./include/
SRC			= $(wildcard $(SRC_PATH)/*.cpp) $(wildcard $(SRC_PATH)/*/*.cpp) # CHANGE THIS BEFORE EVALUATION!! We can't use wildcards
SRCS		= $(SRC)
OBJ			= $(patsubst $(SRC_PATH)%, %, $(SRCS:.cpp=.o))
OBJS		= $(addprefix $(OBJ_PATH), $(OBJ))
INC			= -I $(INC_PATH)

# Main rule
all: $(OBJ_PATH) $(NAME)

# Objects directory rule
$(OBJ_PATH):
	mkdir -p $(OBJ_PATH)

# Objects rule
$(OBJ_PATH)%.o: $(SRC_PATH)%.cpp
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
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --track-fds=yes --trace-children=yes --num-callers=20 --log-file=valgrind_out.txt ./ircserv 8080 p

.PHONY: all re clean fclean valgrind