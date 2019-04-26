#include "TcpClient.h" // TODO

using std::vector;

vector<unsigned char> TcpClient::Request(const TcpMsg& tcpMsg, int port) {
	vector<unsigned char> buf;

	int sockfd, numbytes;
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(tcpMsg.host.c_str(), std::to_string(port).c_str(), &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		throw 1;
	}

	// loop through all the results and connect to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
							 p->ai_protocol)) == -1) {
			perror("client: socket");
			continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			perror("client: connect");
			close(sockfd);
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "client: failed to connect\n");
		throw 2;
	}

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
			  s, sizeof s);
	printf("client: connecting to %s\n", s);

	freeaddrinfo(servinfo); // all done with this structure

	if (send(sockfd, &tcpMsg.msg[0], tcpMsg.msg.size(), 0) == -1)
		perror("send");


	vector<unsigned char> tmpBuf;
	tmpBuf.resize(MAXBUFLEN);

	while ((numbytes = recv(sockfd, &tmpBuf[0], tmpBuf.size(), 0)) > 0) {
		tmpBuf.resize(numbytes);
		buf.insert(end(buf),begin(tmpBuf),end(tmpBuf));
		tmpBuf.clear();
		tmpBuf.resize(MAXBUFLEN);
	}

	if (numbytes == -1) {
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
