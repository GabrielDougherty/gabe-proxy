#pragma once
#include <vector>
#include <csignal>
#include <regex>
#include <sstream>
#include <numeric>
#include "TcpServer.h"

struct TcpMsg {
	std::vector<unsigned char> msg;
	std::string host;
};

class ProxyServer {
 public:
	ProxyServer(TcpServer *serv);
	void Listen();
 private:
	/* static std::string CharVec2String(const std::vector<unsigned char>&); */
	/* static std::vector<unsigned char> String2CharVec(std::string); */
	static TcpMsg DeproxifyMsg(const std::vector<unsigned char>&);
	static void SendResponse(const int, const int,
							 const std::vector<unsigned char>&);
	TcpServer* _tcpServer;
};
