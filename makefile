CXX:=clang++
CXXFLAGS:=-std=c++17 -Wall -Wpedantic -Wuninitialized -g -fPIC
CXXFLAGSTARGET:=$(CXXFLAGS) -lcrypto

#all: proxyServer TcpServer.o ProxyServer.o TcpClient.o

proxyServer: TcpServer.o ProxyServer.o TcpClient.o
	$(CXX) proxyServer.cpp *.o -o proxyServer $(CXXFLAGSTARGET)

ProxyServer.o: TcpServer.o

clean:
	rm proxyServer *.o




