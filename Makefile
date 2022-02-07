.PHONY:	all clean fclean

SRC_PARSER  = parser.cpp parser_main.cpp
SRC_TEST = listen_test.cpp listen_main.cpp parser.cpp

OBJ_PARSER  = $(SRC_PARSER:%.cpp=%.o)
OBJ_TEST	= $(SRC_TEST:%.cpp=%.o)

HEADER_PARSER = parser.hpp
HEADER_TEST = listen_test.hpp

CXX	= clang++
CXXFLAGS = -Wall -Werror -Wextra -std=c++98 -g3

PARSER = parser
TEST = test

$(PARSER):		$(OBJ_PARSER) $(HEADER_PARSER)
	@$(CXX) $(CXXFLAGS) -o $(PARSER) $(OBJ_PARSER)
	@echo "Parser created."

$(TEST):		$(OBJ_TEST) $(HEADER_TEST)
	@$(CXX) $(CXXFLAGS) -o $(TEST) $(OBJ_TEST)
clean:
	rm -rf $(PARSER) $(TEST)

fclean:		clean
	rm -rf $(OBJ_PARSER) $(OBJ_TEST)
