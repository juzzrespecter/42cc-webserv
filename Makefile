.PHONY:	all clean fclean

SRC_PARSER  = parser.cpp parser_main.cpp

OBJ_PARSER  = $(SRC_PARSER:%.cpp=%.o)

HEADER_PARSER = parser.hpp

CXX	= clang++
CXXFLAGS = -Wall -Werror -Wextra -std=c++98 -g3

PARSER = parser

$(PARSER):		$(OBJ_PARSER) $(HEADER_PARSER)
	@$(CXX) $(CXXFLAGS) -o $(PARSER) $(OBJ_PARSER)
	@echo "Parser created."

clean:
	rm -rf $(PARSER)

fclean:		clean
	rm -rf $(OBJ_PARSER)
