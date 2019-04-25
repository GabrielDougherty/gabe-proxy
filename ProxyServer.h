#pragma once
#include <vector>
#include <csignal>
#include "TcpServer.h"

class ProxyServer {
 public:
	ProxyServer(TcpServer *serv);
	void Listen();
 private:
	static void SendResponse(const int, const int,
							 const std::vector<unsigned char>&);
	TcpServer* _tcpServer;
};
