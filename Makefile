NAME = ircserv
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -Iinclude -g
SRCS = main.cpp src/Client/Client.cpp src/Commands/Command.cpp src/Channel/Channel.cpp src/Server/StartServer.cpp \
		src/Server/ServerHelpers.cpp src/Server/ServerEvents.cpp src/Server/ServerClientUtils.cpp \
		src/Server/ServerChannelUtils.cpp src/Server/GraceFullShutDown.cpp 
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
	@echo "$(GREEN)✔ Successfully compiled $(NAME)$(RESET)"

irc_bot: bot/irc_bot.cpp
	$(CXX) -std=c++98 -Wall -Wextra -o irc_bot bot/irc_bot.cpp

clean:
	rm -rf obj
	@echo "$(RED)✔ Successfully cleaned object files$(RED)$(RESET)"
fclean: clean
	@rm -f $(NAME)
	@rm -f irc_bot
	@echo "$(RED)✔ Successfully cleaned executable $(RED)$(RESET)"

re: fclean all