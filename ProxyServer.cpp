#include "ProxyServer.h"

using namespace std;

ProxyServer::ProxyServer(TcpServer* serv) : _tcpServer(serv) { }

void ProxyServer::Listen() {
	_tcpServer->Listen([](const int sockfd, int new_fd, const TcpResult& result) {
						   for (auto& c: result.msg)
							   cout << c;
						   cout << endl;
						   // TODO do proxy request and caching

						   SendResponse(sockfd, new_fd, result.msg);
					   });
}

// send response back to client
void ProxyServer::SendResponse(const int sockfd,
					   const int new_fd,
					   const vector<unsigned char>& msg) {
	if (!fork()) { // this is the child process
		close(sockfd); // child doesn't need the listener

		// TODO: change from simple echo
		if (send(new_fd, &msg[0], msg.size(), 0) == -1)
			perror("send");
		close(new_fd);
		exit(0);
	}
}
