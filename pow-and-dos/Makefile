CXXFLAGS = -std=c++17 -Wall -Wextra -O3 -DNDEBUG
LIBS = -pthread


all: server

server: server.cpp
	g++ $(CXXFLAGS) $(LIBS) server.cpp -o server

clean:
	rm -f server
	rm -rf __pycache__
