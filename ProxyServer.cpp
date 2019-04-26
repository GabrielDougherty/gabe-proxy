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

						   cout << "REQUEST FROM CLIENT:" << endl;

						   for (auto& c: result.msg)
							   cout << c;
						   cout << endl;

						   TcpMsg filteredMsg = DeproxifyMsg(result.msg);

						   cout << "SENDING TO HOST:" << filteredMsg.host << endl;

						   cout << "SENDING REQUEST TO HOST:" << endl;
						   for (auto& c: filteredMsg.msg)
							   cout << c;
						   cout << endl;

						   cout << "DOING CACHE CHECK:" << endl;

						   auto buf = CacheOrRequest(filteredMsg);

						   cout << "SENDING RESPONSE:" << endl;

						   SendResponse(sockfd, new_fd, buf);
					   });
}

vector<unsigned char> ProxyServer::CacheOrRequest(const TcpMsg& tcpRequest)
{
	std::string request(begin(tcpRequest.msg), end(tcpRequest.msg));
	// check file md5 of request
	unsigned char md5name[MD5_DIGEST_LENGTH];
	vector<unsigned char> result;

	MD5((unsigned char*)request.c_str(), request.size(), md5name);
	
	std::string sfname;
	char buf[32];
	for (int i=0;i<16;i++){
		sprintf(buf, "%02x", md5name[i]);
		sfname.append( buf );
	}

	auto fname = sfname.c_str();

	std::fstream cacheFile;

	std::cout << "Trying cache with " << fname << std::endl;
	cacheFile.open(fname, std::ios::in | std::ios::binary);
	if (cacheFile) {
		std::cout << "Using cache" << std::endl;
		
		// unsigned char tmp;
		// while (cacheFile >> tmp) {
		// 	result.push_back(tmp);
		// }

		result = std::vector<unsigned char> 
			((std::istreambuf_iterator<char>(cacheFile)),
			 std::istreambuf_iterator<char>());

	} else {
		std::cout << "Requesting from remote HTTP server" << std::endl;

		result = _tcpClient->Request(tcpRequest, 80);

		cacheFile.open(fname, std::ios::out | std::ios::binary);

		cacheFile.write((const char *)result.data(), result.size());

		// for (auto& c : result)
		// 	cacheFile << c;
	}
	return result;
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
