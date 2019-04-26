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

						   cout << "FILTERED:" << endl;
						   cout << "filteredMsg.host: " << filteredMsg.host 
								<< endl;
						   for (auto& c: filteredMsg.msg)
							   cout << c;
						   cout << endl;


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

	bufStream >> url;
	regex urlMatch(url);

	if (url == "/"s)
		throw 1;

	// remove leading '/' or leading http:// (this is fairly fragile)
	regex httpLeaderMatch("^/?http://.*$");
	regex slashLeaderMatch("^/.*$");
	if (regex_match(begin(url), end(url), httpLeaderMatch)) {
		regex httpLeader("http://");
		url = regex_replace(url, httpLeader, ""s);
		if (regex_match(begin(url), end(url), slashLeaderMatch))
			url.erase(begin(url));
	} else if (regex_match(begin(url), end(url), slashLeaderMatch)) {
		url.erase(begin(url));
	}  else {
		throw ""; // malformed request
	}

	// Check whether GET URL has a trailing path
	regex justHost("^.*/.*$");
	std::string replaced;
	if (regex_match(begin(url), end(url), justHost)) {
		std::stringstream pathStream(url);

		string urlPath;

		using std::getline;

		getline(pathStream, tcpMsg.host, '/');
		getline(pathStream, urlPath);

		replaced = regex_replace(bufString, urlMatch, "/"s + urlPath);
	} else {
		replaced = regex_replace(bufString, urlMatch, "/"s);
	}
	regex hostMatch("\nHost:.*\r");
	replaced = regex_replace(replaced, hostMatch, "\nHost: "s + tcpMsg.host
							 + "\r"s);
	tcpMsg.msg = vector<unsigned char>(begin(replaced), end(replaced));


	return tcpMsg;
}

// send response back to client
void ProxyServer::SendResponse(const int sockfd,
							   const int new_fd,
							   const vector<unsigned char>& msg) {
	if (!fork()) { // this is the child process
		close(sockfd); // child doesn't need the listener


		for (size_t i = 0; i < msg.size(); i += TcpClient::MAXBUFLEN) {
			// size_t sendSize = std::min((msg.size()-i), 
			// 				  static_cast<size_t>(TcpClient::MAXBUFLEN));
			if (send(new_fd, &msg[i], msg.size()-i, 0) == -1)
				perror("send");
		}

		close(new_fd);
		exit(0);
	}
}
