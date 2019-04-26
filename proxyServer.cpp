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

}
