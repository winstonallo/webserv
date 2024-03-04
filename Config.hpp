#pragma once

#include <string>
#include <vector>

typedef std::string string;

class Config
{
	private:
		int						_port; 
		std::vector<string> 	_domain_names;
		string					_acces_log_path;
	public:
		Config(const string &path="default_config.conf");
		void load_config(const string &path);
};