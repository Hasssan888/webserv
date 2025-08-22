NAME = webserv
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98

SRC = main.cpp \
      config/ConfigParser.cpp \
      config/ServerConfig.cpp \
      config/LocationConfig.cpp \
      cor/ServerManager.cpp \
      cor/ServerSocket.cpp \
      cor/ClientConnection.cpp \
      Http/HttpResponse.cpp \
      Http/HttpRequest.cpp \
      CGI/CgiHandler.cpp

OBJ = $(SRC:.cpp=.o)

# Directories to create
DIRS = www www/upload

all: $(DIRS) $(NAME)

$(DIRS):
	@mkdir -p $@

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

cor/ClientConnection.o: cor/ClientConnection.cpp cor/ClientConnection.hpp Http/HttpRequest.hpp Http/HttpResponse.hpp CGI/CgiHandler.hpp
	$(CXX) $(CXXFLAGS) -c cor/ClientConnection.cpp -o cor/ClientConnection.o

Http/HttpRequest.o: Http/HttpRequest.cpp Http/HttpRequest.hpp
	$(CXX) $(CXXFLAGS) -c Http/HttpRequest.cpp -o Http/HttpRequest.o

Http/HttpResponse.o: Http/HttpResponse.cpp Http/HttpResponse.hpp
	$(CXX) $(CXXFLAGS) -c Http/HttpResponse.cpp -o Http/HttpResponse.o

CGI/CgiHandler.o: CGI/CgiHandler.cpp CGI/CgiHandler.hpp Http/HttpRequest.hpp Http/HttpResponse.hpp config/LocationConfig.hpp
	$(CXX) $(CXXFLAGS) -c CGI/CgiHandler.cpp -o CGI/CgiHandler.o

install-files: $(DIRS)
	@echo "Installing test files..."
	@if [ ! -f www/index.html ]; then \
		echo '<!DOCTYPE html><html><head><title>Webserv Test</title><style>body{font-family:Arial;max-width:800px;margin:0 auto;padding:20px;background:linear-gradient(135deg,#667eea,#764ba2);color:white;min-height:100vh}.container{background:rgba(255,255,255,0.1);padding:30px;border-radius:15px;backdrop-filter:blur(10px)}.button{display:inline-block;padding:12px 24px;background:linear-gradient(45deg,#ff6b6b,#ee5a52);color:white;text-decoration:none;border-radius:25px;margin:5px}</style></head><body><div class="container"><h1>🚀 Webserv Test Page</h1><p>Welcome to the Webserv HTTP server!</p><h2>Test Links:</h2><a href="/test.php" class="button">Test CGI (PHP)</a><a href="/upload" class="button">Upload Area</a><a href="/videos" class="button">Videos</a><h2>Upload Test:</h2><form action="/upload" method="post" enctype="multipart/form-data"><input type="file" name="upload_file"><button type="submit" class="button">Upload</button></form></div></body></html>' > www/index.html; \
	fi
	@if [ ! -f www/test.php ]; then \
		echo '<?php header("Content-Type: text/html"); ?><html><head><title>PHP CGI Test</title></head><body><h1>PHP CGI Working!</h1><p>Server: <?php echo $$_SERVER["SERVER_SOFTWARE"] ?? "webserv/1.0"; ?></p><p>Method: <?php echo $$_SERVER["REQUEST_METHOD"]; ?></p><p>URI: <?php echo $$_SERVER["REQUEST_URI"]; ?></p><p>Time: <?php echo date("Y-m-d H:i:s"); ?></p><a href="/">Back to home</a></body></html>' > www/test.php; \
		chmod +x www/test.php; \
	fi

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

test: $(NAME) install-files
	@echo "🧪 Running webserv..."
	@echo "📁 Test files installed in www/"
	@echo "🌐 Open http://localhost:9090 in your browser"
	./$(NAME)

.PHONY: all clean fclean re test install-files