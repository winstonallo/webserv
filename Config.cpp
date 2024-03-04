#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

Config::Config(const string &path)
{
	load_config(path);
}

void Config::load_config(const string &path)
{
	string config;
	std::ifstream config_file(path.c_str());
	if (config_file.is_open() == true)
	{
		std::ostringstream out_string;
		out_string << config_file.rdbuf();
		config = out_string.str();
	}
	std::cout << config;
}