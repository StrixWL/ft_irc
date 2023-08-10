NAME=			ircserv
FLAGS=			#-Wall -Werror -Wextra
CC=				c++
HEADERS=		include/Client.hpp \
				include/Logger.hpp \
				include/Server.hpp \
				include/Channel.hpp
BONUS_HEADERS=	bonus/bot/bot.hpp
SRC_DIR=		src
SRC_FILES=		server.cpp \
				src/Client.cpp \
				src/Logger.cpp \
				src/Commands.cpp \
				src/Channel.cpp

SRC_FILES_BONUS= src/Logger.cpp \
				 bonus/bot/bot.cpp


OBJ_FILES_BONUS=		$(SRC_FILES_BONUS:.cpp=.o)
OBJ_FILES=		$(SRC_FILES:.cpp=.o)


all: $(NAME)


$(NAME): $(OBJ_FILES)
	$(CC) $(OBJ_FILES) -o $(NAME) -std=c++98 -Iinclude

%.o: %.cpp $(HEADERS)
	$(CC) $(FLAGS) -Iinclude -Ibonus/bot -c $< -o $@ -std=c++98

bonus : $(OBJ_FILES_BONUS) $(HEADERS)
	$(CC) $(OBJ_FILES_BONUS) -o bot -std=c++98 -Iinclude

clean:
	rm -f $(OBJ_FILES) $(OBJ_FILES_BONUS)

fclean: clean
	rm -f $(NAME) bot

re: fclean all