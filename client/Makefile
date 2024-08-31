BLUE = \033[0;94m
GREEN = \e[1;92m
RED = \033[0;91m
RESET = \033[1;30m
WHITE = \033[1;97m
YELLOW = \033[1;33m
EMOJI_PACKAGE = \360\237\223\246
EMOJI_HAMMER = \360\237\224\250
EMOJI_TRASH = \360\237\227\221\357\270\217

NAME = client

CC = c++
CFLAGS = -O0 -std=c++98

SRC = $(wildcard $(SRC_PATH)*.cpp $(SRC_PATH)**/*.cpp)

all: $(NAME)

$(NAME): $(SRC)
	@$(CC) $(CFLAGS) -o $(NAME) $(SRC)
	@printf  "\n$(EMOJI_PACKAGE)	$(WHITE)$(NAME)				$(YELLOW)compiled$(WHITE)\n"


.PHONY: all