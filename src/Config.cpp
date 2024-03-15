#include "Config.hpp"

void	Config::set_servers(std::map <int, std::map <std::string, std::vector <std::string> > >& raw_servers)
{
	for (std::map <int, std::map <std::string, std::vector <std::string> > >::iterator it = raw_servers.begin(); it != raw_servers.end(); it++)
	{
		ServerInfo* new_server = new ServerInfo();

		new_server->set_server_name(it->second["server_name"]);
		new_server->set_port(std::atoi(it->second["port"][0].c_str()));
		new_server->set_access_log(handle_access_log(it->second["access_log"][0]));
		new_server->set_host(it->second["host"][0]);

		_servers.push_back(new_server);
	}
}

// validates access log from config
//
// if:	the file does not exist (meaning we can just create it)
// or:	it exists and we have write access to it
//		->	config valid, return the filename
//
// else:
//		->	path invalid, log error & return default 
std::string		Config::handle_access_log(const std::string& access_log)
{
	if (Utils::file_exists(access_log) == false or Utils::write_access(access_log) == true)
	{
		return access_log;
	}
	else
	{
		Log::log("error: access log '" + access_log + "' could not be opened, falling back to 'access.log'", STD_ERR);

		return ACCESS_LOG_DEFAULT;
	}
}

int		handle_client_max_body_size(const std::string& client_max_body_size)
{
	int	size = Utils::parse_client_max_body_size(client_max_body_size);

	if (size != -1)
	{
		return size;
	}
	else
	{
		Log::log("error: client max body size '" + client_max_body_size + "' is not valid, falling back to default (1M)\n", STD_ERR);
		return CLIENT_MAX_BODY_SIZE_DEFAULT;
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

Config::~Config() 
{
	for (std::vector <ServerInfo *>::iterator it = _servers.begin(); it != _servers.end(); it++)
	{
		delete *it;
	}
}

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