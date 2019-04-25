#include <sstream>
#include <iostream>
#include <memory>
#include "TcpServer.h"
#include "ProxyServer.h"

using namespace std;

int main() {
	const int SERVPORT = 4950;
	// TcpServer serv(SERVPORT);

	auto tcpServ = make_unique<TcpServer>(SERVPORT);
	ProxyServer serv(tcpServ.get());
	serv.Listen();
}
