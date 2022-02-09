.PHONY:	all clean fclean

SRC_TEST    = config_blocks.cpp parser.cpp webserver.cpp server.cpp location.cpp socket.cpp main.cpp
OBJ_TEST	= $(SRC_TEST:%.cpp=%.o)
HEADER_TEST = config_blocks.hpp parser.hpp webserver.hpp server.hpp location.hpp

CXX	= clang++
CXXFLAGS = -Wall -Werror -Wextra -std=c++98 -g3

TEST = test

$(TEST):		$(OBJ_TEST) $(HEADER_TEST)
	@$(CXX) $(CXXFLAGS) -o $(TEST) $(OBJ_TEST)
clean:
	rm -rf $(PARSER) $(TEST)

fclean:		clean
	rm -rf $(OBJ_PARSER) $(OBJ_TEST)
