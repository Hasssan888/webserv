NAME = webserv

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -g3 -fsanitize=address

SRCDIRS = core http config cgi utils
SRCFILES = main.cpp $(foreach dir,$(SRCDIRS),$(wildcard $(dir)/*.cpp))
OBJFILES = $(SRCFILES:.cpp=.o)

INCLUDES = -I.

all: $(NAME)

$(NAME): $(OBJFILES)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJFILES)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJFILES)

fclean: clean
	rm -f $(NAME)

re: fclean all
