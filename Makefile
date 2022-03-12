.PHONY:	all clean fclean

SRC = config_blocks.cpp \
		parser.cpp \
		server.cpp \
		location.cpp \
		socket.cpp \
		msg_format/Body.cpp \
		msg_format/RequestLine.cpp \
		msg_format/StatusLine.cpp \
		request.cpp \
		response.cpp \
		Cgi.cpp \
		webserver.cpp \
		main.cpp
OBJ	= $(SRC:%.cpp=%.o)
HEADER = config_blocks.hpp parser.hpp webserver.hpp server.hpp location.hpp
CXX	= clang++
CXXFLAGS = -Wall -Werror -Wextra -std=c++98 -g3 -fstandalone-debug

NAME = webserver

all: $(NAME)

$(NAME):		$(OBJ) $(HEADER) Makefile
	@$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJ)
clean:
	rm -rf $(NAME)

fclean:		clean
	rm -rf $(OBJ)

re:	fclean all