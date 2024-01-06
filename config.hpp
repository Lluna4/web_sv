#include <iostream>
#include <fcntl.h>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include "tokenize.hpp"

class url
{
	public:
		url() {}

		url(std::string url, std::string direction, int mode)
			:url_(url), direction_(direction), mode_(mode)
			{}
		
		std::string get_url()
		{
			return url_;
		}
		std::string get_direction()
		{
			return direction_;
		}
		int get_mode()
		{
			return mode_;
		}

		void set_url(std::string new_url)
		{
			url_ = new_url;
		}
		void set_direction(std::string new_direction)
		{
			direction_ = new_direction;
		}
		void set_mode(int new_mode)
		{
			mode_ = new_mode;
		}
	private:
		std::string url_;
		std::string direction_;
		int mode_;
};

std::map<std::string, url> urls;

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

void load_url_config()
{
	std::ifstream infile("urls.cfg");
	std::string line;
	while (std::getline(infile, line))
	{
		if (line.find("//") == std::string::npos)
		{
			url buff;
			std::vector<std::string> a = tokenize(line, ':');
			buff.set_url(a[0]);
			buff.set_direction(a[1]);
			if (a[1].find(".html") != std::string::npos)
				buff.set_mode(0);
			else
				buff.set_mode(1);
			urls.insert({a[0], buff});
			std::cout << "New url " << buff.get_url() << " goes to " << buff.get_direction() << std::endl;
		}
	}
}
