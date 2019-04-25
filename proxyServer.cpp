#include <iostream>
#include "TcpServer.h"

using namespace std;

int main() {
	TcpServer serv;
	serv.Listen([](const int new_fd, const TcpResult& result) {
					for (auto& c: result.msg)
						cout << c;
					cout << endl;
				});
}
