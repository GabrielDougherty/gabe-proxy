#pragma once
#include <vector>
#include <csignal>
#include <regex>
#include <sstream>
#include <numeric>
#include <fstream>
#include <iomanip>
#include <openssl/md5.h>
#include "TcpServer.h"
#include "TcpMsg.h"
#include "TcpClient.h"

class ProxyServer {
 public:
	ProxyServer(TcpServer*, TcpClient*);
	void Listen();
 private:
	/* static std::string CharVec2String(const std::vector<unsigned char>&); */
	/* static std::vector<unsigned char> String2CharVec(std::string); */
	static TcpMsg DeproxifyMsg(const std::vector<unsigned char>&);
	static void SendResponse(const int, const int,
							 const std::vector<unsigned char>&);
	std::vector<unsigned char> CacheOrRequest(const TcpMsg&);

	TcpServer* _tcpServer;
	TcpClient* _tcpClient;
};
