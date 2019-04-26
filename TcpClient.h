#pragma once
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#include <vector>
#include <string>
#include "TcpMsg.h"

class TcpClient {
public:
	std::vector<unsigned char> Request(const TcpMsg&, int);
	// get sockaddr, IPv4 or IPv6:
	static void *get_in_addr(struct sockaddr *sa);
private:
	const int MAXBUFLEN = 4096;
};

