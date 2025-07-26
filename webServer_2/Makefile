NAME = webserv
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98

SRC = main.cpp \
      config/ConfigParser.cpp \
      config/ServerConfig.cpp \
      config/LocationConfig.cpp \
      cor/ServerManager.cpp \
      cor/ServerSocket.cpp \
      cor/ClientConnection.cpp

OBJ = $(SRC:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJ)

# Individual compilation rules with dependencies
main.o: main.cpp config/ConfigParser.hpp cor/ServerManager.hpp
	$(CXX) $(CXXFLAGS) -c main.cpp -o main.o

config/ConfigParser.o: config/ConfigParser.cpp config/ConfigParser.hpp config/ServerConfig.hpp
	$(CXX) $(CXXFLAGS) -c config/ConfigParser.cpp -o config/ConfigParser.o

config/ServerConfig.o: config/ServerConfig.cpp config/ServerConfig.hpp config/LocationConfig.hpp
	$(CXX) $(CXXFLAGS) -c config/ServerConfig.cpp -o config/ServerConfig.o

config/LocationConfig.o: config/LocationConfig.cpp config/LocationConfig.hpp
	$(CXX) $(CXXFLAGS) -c config/LocationConfig.cpp -o config/LocationConfig.o

cor/ServerManager.o: cor/ServerManager.cpp cor/ServerManager.hpp cor/ServerSocket.hpp cor/ClientConnection.hpp
	$(CXX) $(CXXFLAGS) -c cor/ServerManager.cpp -o cor/ServerManager.o

cor/ServerSocket.o: cor/ServerSocket.cpp cor/ServerSocket.hpp config/ServerConfig.hpp
	$(CXX) $(CXXFLAGS) -c cor/ServerSocket.cpp -o cor/ServerSocket.o

cor/ClientConnection.o: cor/ClientConnection.cpp cor/ClientConnection.hpp
	$(CXX) $(CXXFLAGS) -c cor/ClientConnection.cpp -o cor/ClientConnection.o

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re