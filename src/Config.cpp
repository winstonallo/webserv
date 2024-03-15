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
		new_server->set_access_log(handle_access_log(it->second["access_log"][0]));
		new_server->set_host(it->second["host"][0]);
		if (Utils::parse_client_max_body_size(it->second["client_max_body_size"][0]) != -1)
		{
			new_server->set_client_max_body_size(Utils::parse_client_max_body_size(it->second["client_max_body_size"][0]));
		}
		else
		{
			Log::log("error: client max body size '" + it->second["client_max_body_size"][0] + "' is not valid, falling back to default (1M)\n", STD_ERR);
			new_server->set_client_max_body_size(CLIENT_MAX_BODY_SIZE_DEFAULT);
		}
		_servers.push_back(new_server);
	}
}

// validates 
std::string		Config::handle_access_log(const std::string& access_log)
{
	if (Utils::file_exists(access_log) == false or Utils::write_access(access_log) == true)
	{
		return access_log;
	}
	else
	{
		Log::log("error: access log '" + access_log + "' could not be opened, falling back to 'access.log'", STD_ERR);
		
		return ("access.log");
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