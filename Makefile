BLUE = \033[0;94m
GREEN = \e[1;92m
RED = \033[0;91m
RESET = \033[1;30m
WHITE = \033[1;97m
YELLOW = \033[1;33m
EMOJI_PACKAGE = \360\237\223\246
EMOJI_HAMMER = \360\237\224\250
EMOJI_TRASH = \360\237\227\221\357\270\217

NAME = webserv

CXX = c++
CXXFLAGS = -Werror -Wextra -Wall -Wno-unknown-pragmas -O3 -std=c++98 -g $(INC)

SRC_PATH = src/
OBJ_PATH = obj/
LOGS_PATH = logs/
CONFIGS_PATH = configs/

INC = $(addprefix -I, $(shell find . -type d))
SRC = $(wildcard $(SRC_PATH)*.cpp $(SRC_PATH)**/*.cpp) \
		src/singletons/maps/BytesUnitMap.cpp

OBJ = $(SRC:$(SRC_PATH)%.cpp=$(OBJ_PATH)%.o)

all: $(NAME)

$(OBJ_PATH)%.o: $(SRC_PATH)%.cpp | $(OBJ_PATH)
	 @mkdir -p $(dir $@)
	 @$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@
	 @printf "$(EMOJI_HAMMER)	$(BLUE)Compiling $(WHITE)$(NAME)		$(BLUE)%-33s$(WHITE)\r" $(notdir $@)

$(LOGS_PATH):
	@mkdir -p $(LOGS_PATH)
	@touch $(LOGS_PATH)webserv.log

$(CONFIGS_PATH):
	@mkdir -p $(CONFIGS_PATH)
	@touch $(CONFIGS_PATH)default.conf

$(OBJ_PATH):
	@mkdir -p $(OBJ_PATH)

$(NAME): $(OBJ) $(LOGS_PATH) $(CONFIGS_PATH)
	@$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJ)
	@printf  "\n$(EMOJI_PACKAGE)	$(WHITE)$(NAME)				$(YELLOW)compiled$(WHITE)\n"

clean:
	@rm -rf $(OBJ_PATH)
	@rm -rf $(LOGS_PATH)

fclean: clean
	@rm -f $(NAME)
	@printf "$(EMOJI_TRASH)	$(WHITE)$(NAME)				$(RED)removed$(WHITE)\n"

re: fclean all

valgrind: all
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --track-fds=all ./$(NAME)

format:
	@find . -iname "*.cpp" -o -iname "*.hpp" | xargs clang-format -i

.PHONY: all re clean fclean format