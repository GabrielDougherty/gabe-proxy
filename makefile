CC=g++
CXXFLAGS=-std=c++17 -Wall -Wpedantic -Wuninitialized -L/usr/lib -g -lcrypto


all: proxyServer TcpServer.o ProxyServer.o TcpClient.o

proxyServer: TcpServer.o ProxyServer.o TcpClient.o
	g++ proxyServer.cpp *.o -L/usr/lib -lcrypto -o proxyServer

ProxyServer.o: TcpServer.o

clean:
	rm proxyServer *.o




