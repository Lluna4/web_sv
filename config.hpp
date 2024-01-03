#include <iostream>
#include <fcntl.h>
#include <fstream>
#include <string>
#include "tokenize.hpp"

void create_config()
{
	std::ofstream cfg("http.cfg");
	cfg << "//Port the server is listening to\n";
	cfg << "port:8082\n";
	cfg.close();
}

void load_config(int *n)
{
	std::ifstream infile("http.cfg");
	std::string line;
	while (std::getline(infile, line))
	{
		if (line.find("//") == std::string::npos)
		{
			std::vector<std::string> a = tokenize(line, ':');
			if (a[0].compare("port") == 0 && a.size() > 1)
			{
				*n = atoi(a[1].c_str());	
			}
		}
	}
	infile.close();
}
