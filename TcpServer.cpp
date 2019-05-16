#include "TcpServer.h"

using namespace std;


// Synchronous TCP server

// input: server port #
TcpServer::TcpServer(const int serverPortNum) {
	struct addrinfo hints, *servInfo, *p;
	struct sigaction sa;
	int yes=1;
	int rv;

	const string serverPort = to_string(serverPortNum); 

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo(nullptr, serverPort.c_str(), &hints, &servInfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		throw 1;
	}

	// loop through all the results and bind to the first we can
	for(p = servInfo; p != nullptr; p = p->ai_next) {
		if ((sockFd = socket(p->ai_family, p->ai_socktype,
							 p->ai_protocol)) == -1) {
			perror("server: socket");
			continue;
		}

		if (setsockopt(sockFd, SOL_SOCKET, SO_REUSEADDR, &yes,
					   sizeof(int)) == -1) {
			perror("setsockopt");
			exit(1);
		}

		if (bind(sockFd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockFd);
			perror("server: bind");
			continue;
		}

		break;
	}

	freeaddrinfo(servInfo); // all done with this structure

	if (p == nullptr)  {
		fprintf(stderr, "server: failed to bind\n");
		exit(1);
	}

	if (listen(sockFd, BACKLOG) == -1) {
		perror("listen");
		exit(1);
	}

	sa.sa_handler = sigchld_handler; // reap all dead processes
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, nullptr) == -1) {
		perror("sigaction");
		throw 1;
	}
}

TcpServer::~TcpServer() {
	close(sockFd);
	close(newFd);
}

// response: function that takes as input
//     a socket for sending
//     a socket for receiving
//     a TcpResult
void TcpServer::Listen(function<void(int, int, const TcpResult&)> response) {
	char s[INET6_ADDRSTRLEN];
	struct sockaddr_storage theirAddr; // connector's address information
	socklen_t sinSize;
	TcpResult result;
	result.msg.resize(MAXBUFLEN);

	printf("server: waiting for connections...\n");

	while(1) {  // main accept() loop
		sinSize = sizeof theirAddr;
		newFd = accept(sockFd, (struct sockaddr *)&theirAddr, &sinSize);
		if (newFd == -1) {
			perror("accept");
			continue;
		}
		int numbytes;
		if ((numbytes = recv(newFd, result.msg.data(), result.msg.size(), 0)) == -1) {
			perror("recv");
			exit(1);
		}
		result.msg.resize(numbytes);

		inet_ntop(theirAddr.ss_family,
				  get_in_addr((struct sockaddr *)&theirAddr),
				  s, sizeof s);
		printf("server: got connection from %s\n", s);
		

		result.from = s;

		response(sockFd, newFd, result);

		close(newFd);  // parent doesn't need this
	}
}

	
void* TcpServer::get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void TcpServer::sigchld_handler(int s)
{
	(void)s; // quiet unused variable warning

	// waitpid() might overwrite errno, so we save and restore it:
	int saved_errno = errno;

	while(waitpid(-1, nullptr, WNOHANG) > 0);

	errno = saved_errno;
}
