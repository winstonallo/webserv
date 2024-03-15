#include "Config.hpp"
#include "Log.hpp"
#include "ServerInfo.hpp"
#include "Director.hpp"
#include "Utils.hpp"


void	Config::set_servers(std::map <int, std::map <std::string, std::vector <std::string> > > raw_servers)
{
	for (std::map <int, std::map <std::string, std::vector <std::string> > >::iterator it = raw_servers.begin(); it != raw_servers.end(); it++)
	{
		ServerInfo* new_server = new ServerInfo();

		new_server->set_server_name(it->second["server_name"]);
		new_server->set_port(std::atoi(it->second["port"][0].c_str()));
		if (Utils::write_access(it->second["access_log"][0]) == true)
		{
			new_server->set_access_log(it->second["access_log"][0]);
		}
		else
		{
			Log::log("error: access log '" + it->second["access_log"][0] + "' could not be opened", STD_ERR);
		}
		// new_server->se
	}
}


std::vector <ServerInfo *>	Config::get_servers()
{
	return _servers;
}

std::string	Config::get_error_page(const int key)
{
	return _error_pages[key];
}

Config::Config() {}

Config::~Config() {}

Config::Config(const Config& rhs) {*this = rhs;}

Config	&Config::operator=(const Config& rhs)
{
	if (this != &rhs)
	{
		_servers = rhs._servers;
		_error_pages = rhs._error_pages;
	}
	return *this;
}