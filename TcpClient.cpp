#include "TcpClient.h" // TODO

using std::vector;

vector<unsigned char> TcpClient::Request(const TcpMsg& tcpMsg, int port) {
	vector<unsigned char> buf;

	int sockFd, numBytes;
	struct addrinfo hints, *servInfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(tcpMsg.host.c_str(), std::to_string(port).c_str(), &hints, &servInfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		throw 1;
	}

	// loop through all the results and connect to the first we can
	for(p = servInfo; p != nullptr; p = p->ai_next) {
		if ((sockFd = socket(p->ai_family, p->ai_socktype,
							 p->ai_protocol)) == -1) {
			perror("client: socket");
			continue;
		}

		if (connect(sockFd, p->ai_addr, p->ai_addrlen) == -1) {
			perror("client: connect");
			close(sockFd);
			continue;
		}

		break;
	}

	if (p == nullptr) {
		fprintf(stderr, "client: failed to connect\n");
		throw 2;
	}

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
			  s, sizeof s);
	printf("client: connecting to %s\n", s);

	freeaddrinfo(servInfo); // all done with this structure

	if (send(sockFd, tcpMsg.msg.data(), tcpMsg.msg.size(), 0) == -1)
		perror("send");


	vector<unsigned char> tmpBuf;
	tmpBuf.resize(MAXBUFLEN);

	while ((numBytes = recv(sockFd, tmpBuf.data(), tmpBuf.size(), 0)) > 0) {
		tmpBuf.resize(numBytes);
		buf.insert(end(buf),begin(tmpBuf),end(tmpBuf));
		tmpBuf.clear();
		tmpBuf.resize(MAXBUFLEN);
	}

	if (numBytes == -1) {
		perror("recv");
		throw 1;
	}

	// printf("client: received '%s'\n",buf);

	return buf;
}

// get sockaddr, IPv4 or IPv6:
void* TcpClient::get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
