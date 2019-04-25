CC=g++
CXXFLAGS=-std=c++17 -Wall -Wpedantic -Wuninitialized -g


all: proxyServer TcpServer.o ProxyServer.o

proxyServer: TcpServer.o ProxyServer.o

clean:
	rm proxyServer *.o




