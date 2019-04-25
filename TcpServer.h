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
	TcpServer();
	~TcpServer();
	void Listen(std::function<void(const int, const TcpResult&)>);

private:
	const unsigned int MAXBUFLEN = 100;
	const unsigned int BACKLOG = 10;
	const char *SERVPORT = "4950";
	int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size;
	struct sigaction sa;
	int yes=1;
	char s[INET6_ADDRSTRLEN];
	int rv;

	void *get_in_addr(struct sockaddr *);
	static void sigchld_handler(int s);
};
