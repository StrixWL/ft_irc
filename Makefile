NAME=			server
# FLAGS=			-Wall -Werror -Wextra
CC=				c++
HEADERS=		include/Client.hpp
SRC_DIR=		src
SRC_FILES=		server.cpp \
				src/Client.cpp \
				src/Commands.cpp

OBJ_FILES=		$(SRC_FILES:.cpp=.o)


all: $(NAME)


$(NAME): $(OBJ_FILES)
	$(CC) $(OBJ_FILES) -o $(NAME) -std=c++98

%.o: %.cpp $(HEADERS)
	$(CC) $(FLAGS) -Iinclude -c $< -o $@ -std=c++98

clean:
	rm -f $(OBJ_FILES)

fclean: clean
	rm -f $(NAME)

re: fclean all