CC=g++
CXXFLAGS=-std=c++17 -Wall -Wpedantic -Wuninitialized -g


all: proxyServer TcpServer.o ProxyServer.o TcpClient.o

proxyServer: TcpServer.o ProxyServer.o TcpClient.o
ProxyServer.o: TcpServer.o

clean:
	rm proxyServer *.o




