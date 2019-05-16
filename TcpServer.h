#pragma once
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/wait.h>
#include <csignal>
#include <vector>
#include <iostream>
#include <functional>

// TODO write exception classes
struct TcpResult {
	std::vector<unsigned char> msg;
	std::string from;
};

class TcpServer {
public:
	TcpServer(int);
	virtual ~TcpServer();
	virtual void Listen(std::function<void(int, int, const TcpResult&)>);

	static const unsigned int MAXBUFLEN = 4096;
	static const unsigned int BACKLOG = 10;
private:
	int sockFd, newFd;  // listen on sockFd, new connection on newFd

	void *get_in_addr(struct sockaddr *);
	static void sigchld_handler(int s);
};
