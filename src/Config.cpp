#include "Config.hpp"
#include "Utils.hpp"
#include <string>
#include "Log.hpp"
#include <cstdlib>

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
	if (_error_pages.find(key) != _error_pages.end())
	{
		return _error_pages[key];
	}
	else 
	{
		return generate_default_error_page(key);
	}
}

std::string	Config::generate_default_error_page(const int status_code)
{
	std::string default_error_code = "400", default_error_message = "bad request", default_html = Utils::file_to_string(DEFAULT_ERROR_PAGE);
	std::string	new_error_code = Utils::itoa(status_code), new_error_message = _status_codes[status_code];

	size_t pos_code = default_html.find(default_error_code), pos_message = default_html.find(default_error_message);

	while (pos_code != std::string::npos || pos_message != std::string::npos)
	{
		if (pos_code != std::string::npos)
		{
			default_html.replace(pos_code, default_error_code.size(), new_error_code);
			pos_code = default_html.find(default_error_code, pos_code + new_error_code.size());
		}
		if (pos_message != std::string::npos)
		{
			default_html.replace(pos_message, default_error_message.size(), new_error_message);
			pos_message = default_html.find(default_error_message, pos_message + new_error_message.size());
		}
	}
	std::string new_html_path = "/tmp/" + Utils::itoa(status_code) + ".html";

	std::ofstream	oss(new_html_path.c_str());
	oss << default_html;
	return new_html_path;
}

Config::Config(const std::map <int, std::string> error_pages) : _error_pages(error_pages), _status_codes(Utils::get_error_status_codes()) {}

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