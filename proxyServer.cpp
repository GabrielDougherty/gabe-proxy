#include <sstream>
#include <iostream>
#include <memory>
#include "TcpServer.h"
#include "TcpClient.h"
#include "ProxyServer.h"
#include "TcpMsg.h"

using namespace std;

int main() {
	const int SERVPORT = 4950;

	auto tcpServ = make_unique<TcpServer>(SERVPORT);
	auto tcpClient = make_unique<TcpClient>();
	ProxyServer serv(tcpServ.get(),
					 tcpClient.get());
	serv.Listen();

	// TcpClient client;
	// TcpMsg req;
	// req.host = "www.edinboro.edu";
	// string msg ("GET / HTTP/1.1\r\nHost: www.edinboro.edu\r\n\r\n");
	// req.msg = vector<unsigned char>(begin(msg),end(msg));
	// auto buf = client.Request(req,80);
	// cout << string(begin(buf),end(buf)) << endl;
}
