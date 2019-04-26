#include "ProxyServer.h"

using std::vector;

ProxyServer::ProxyServer(TcpServer* serv,
						 TcpClient* client) :
	_tcpServer(serv), _tcpClient(client) { }

void ProxyServer::Listen() {
	_tcpServer->Listen([this](const int sockfd, int new_fd,
						  const TcpResult& result) {
						   using std::cout;
						   using std::endl;

						   for (auto& c: result.msg)
							   cout << c;
						   cout << endl;
						   // TODO caching

						   TcpMsg filteredMsg = DeproxifyMsg(result.msg);

						   auto buf = _tcpClient->Request(filteredMsg, 80);

						   SendResponse(sockfd, new_fd, buf);
					   });
}

TcpMsg ProxyServer::DeproxifyMsg(const vector<unsigned char>& buf) {
	using std::regex;
	using std::string;
	using namespace std::string_literals;

	TcpMsg tcpMsg;

	string bufString(begin(buf),end(buf));

	std::stringstream bufStream(bufString);
	string httpMethod, url;

	bufStream >> httpMethod;
	regex httpGet("^GET$");
	if (!regex_match(begin(httpMethod), end(httpMethod), httpGet))
		throw 1;

	// Check whether GET URL has a follwing path
	bufStream >> url;
	regex urlMatch(url);

	regex justHost("^/.*/.*$");
	std::string replaced;
	if (regex_match(begin(url), end(url), justHost)) {
		std::stringstream pathStream(url);

		string urlPath;

		using std::getline;

		pathStream.get(); // discard first '/'
		getline(pathStream, tcpMsg.host, '/');
		getline(pathStream, urlPath);

		replaced = regex_replace(bufString, urlMatch, "/"s + urlPath);
	} else {
		tcpMsg.host = string(begin(url)+1,end(url)); // discard first '/'


		replaced = regex_replace(bufString, urlMatch, "/"s);
	}
	regex hostMatch("^Host:.*$");
	replaced = regex_replace(replaced, hostMatch, "Host: " + tcpMsg.host);
	tcpMsg.msg = vector<unsigned char>(begin(replaced), end(replaced));


	return tcpMsg;
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
