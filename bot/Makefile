NAME = bot
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -Iinclude -g
SRCS = main.cpp src/Bot.cpp
OBJS = $(SRCS:%.cpp=obj/%.o)

GREEN = \033[0;32m
RESET = \033[0m
RED = \033[0;31m

# Compilation rule
obj/%.o: %.cpp
	@mkdir -p $(@D)
	@$(CXX) $(CXXFLAGS) -c $< -o $@

all: $(NAME)

$(NAME): $(OBJS)
	@$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)
	@echo "$(GREEN)boBot$(NAME)$(RESET)"

clean:
	rm -rf obj
	@echo "$(RED)bibyt$(RED)$(RESET)"
fclean: clean
	@rm -f $(NAME)
	@rm -f irc_bot
	@echo "$(RED)bibot$(RED)$(RESET)"

re: fclean all