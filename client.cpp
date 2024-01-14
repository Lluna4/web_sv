#include <iostream>
#include "tokenize.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char *argv[])
{
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in addr;
	char *buff;


	std::vector<std::string> a = tokenize("127.0.0.1:8081", '/');
	if (a[0].find(".") != std::string::npos)
	{
		std::vector<std::string> b = tokenize(a[0], ':');
		inet_pton(AF_INET, b[0].c_str(), &addr.sin_addr);
		if (b.size() < 2)
			addr.sin_port = htons(80);
		else
		{
			addr.sin_port = htons(atoi(b[1].c_str()));
		}
		addr.sin_family = AF_INET;
	}
	if (a.size() < 2)
	{
		connect(sock, (struct sockaddr *)&addr, sizeof(addr));
		std::string req = "GET / HTTP/1.1";
		send(sock, req.c_str(), req.length(), 0);
	}
}
