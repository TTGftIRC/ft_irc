NAME = ircserve
CC = c++
CFLAGZ = -Wall -Wextra -Werror -std=c++98 -Iinclude -g
SRCS = main.cpp src/Server.cpp
OBJS = $(SRCS:.cpp=.o)

GREEN = \033[0;32m
RESET = \033[0m
RED = \033[0;31m

# Compilation rule
%.o: %.cpp
	@$(CC) $(CFLAGZ) -c $< -o $@

all: $(NAME)

$(NAME): $(OBJS)
	@$(CC) $(CFLAGZ) -o $(NAME) $(OBJS)
	@echo "$(GREEN)✔ Successfully compiled $(NAME)$(RESET)"

clean:
	@rm -f $(OBJS)
	@echo "$(RED)✔ Successfully cleaned object files$(RED)"

fclean: clean
	@rm -f $(NAME)
	@echo "$(RED)✔ Successfully cleaned executable $(RED)"

re: fclean all
