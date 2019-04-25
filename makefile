CC=g++
CXXFLAGS=-std=c++17

all: proxyServer TcpServer.o

proxyServer: TcpServer.o

clean:
	rm proxyServer *.o




