NAME = ircserve
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -Iinclude -g
SRCS = main.cpp src/Client.cpp src/Command.cpp src/Server.cpp src/Channel.cpp
OBJS = $(SRCS:.cpp=.o)

GREEN = \033[0;32m
RESET = \033[0m
RED = \033[0;31m

# Compilation rule
%.o: %.cpp
	@$(CXX) $(CXXFLAGS) -c $< -o $@

all: $(NAME)

$(NAME): $(OBJS)
	@$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)
	@echo "$(GREEN)✔ Successfully compiled $(NAME)$(RESET)"

clean:
	@rm -f $(OBJS)
	@echo "$(RED)✔ Successfully cleaned object files$(RED)$(RESET)"

fclean: clean
	@rm -f $(NAME)
	@echo "$(RED)✔ Successfully cleaned executable $(RED)$(RESET)"

re: fclean all
