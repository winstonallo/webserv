#include "Config.hpp"
#include "ServerInfo.hpp"
#include "Utils.hpp"
#include <algorithm>
#include <exception>
#include <netinet/in.h>
#include <stdexcept>
#include <string>
#include "Log.hpp"
#include <cstdlib>
#include <sys/socket.h>
#include <vector>
#include <arpa/inet.h>

void	Config::set_servers(std::map <int, std::map <std::string, std::vector <std::string> > >& raw_servers)
{
	ServerInfo* new_server;

	for (std::map <int, std::map <std::string, std::vector <std::string> > >::iterator it = raw_servers.begin(); it != raw_servers.end(); it++)
	{
		try
		{	
			new_server = new ServerInfo();

			handle_server_names(it->second["server_name"], new_server);
			handle_port(std::atoi(it->second["port"][0].c_str()), new_server);
			new_server->set_access_log(handle_access_log(it->second["access_log"][0]));
			handle_host(it->second["host"][0], new_server);
			new_server->set_client_max_body_size(handle_client_max_body_size(it->second["client_max_body_size"][0]));

			_servers.push_back(new_server);
		}
		catch (const std::exception& e)
		{
			delete new_server;
			Log::log(e.what(), STD_ERR | ERROR_FILE);
			continue ;
		}
	}
}

void	Config::handle_host(std::string& hostname, ServerInfo* new_server)
{
	struct in_addr	ip_address;

	if (hostname == "localhost")
	{
		hostname = "127.0.0.1";
	}
	if (std::find(_hosts.begin(), _hosts.end(), hostname) != _hosts.end())
	{
		throw std::runtime_error("error: '" + hostname + "': host address already taken, server will not be initialized\n");
	}
	else if (inet_pton(AF_INET, hostname.c_str(), &ip_address) != 1)
	{
		throw std::runtime_error("error: '" + hostname + "' is not a valid IPv4 address, server will not be initialized\n");
	}
	_hosts.push_back(hostname);

	new_server->set_host_address(ip_address);
}

void	Config::handle_server_names(const std::vector <std::string>& new_server_names, ServerInfo* new_server)
{
	for (std::vector <std::string>::const_iterator it = new_server_names.begin(); it != new_server_names.end(); it++)
	{
		if (std::find(_server_names.begin(), _server_names.end(), *it) != _server_names.end())
		{
			throw std::runtime_error("error: on server: '" + *it + "': name already taken, server will not be initialized\n");
		}
	}
	new_server->set_server_name(new_server_names);
	_server_names.insert(_server_names.end(), new_server_names.begin(), new_server_names.end());
}

void	Config::handle_port(const int port, ServerInfo* new_server)
{
	if (std::find(_ports.begin(), _ports.end(), port) != _ports.end())
	{
		throw std::runtime_error("error: on server '" + new_server->get_server_name()[0] + "': port " + Utils::itoa(port) + " already taken: '" + new_server->get_server_name()[0] + "' will not be initialized\n");
	}
	new_server->set_port(port);
	_ports.push_back(port);
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
		Log::log("error: access log '" + access_log + "' could not be opened, falling back to 'access.log'", STD_ERR | ERROR_FILE);

		return ACCESS_LOG_DEFAULT;
	}
}

int		Config::handle_client_max_body_size(const std::string& client_max_body_size)
{
	int	size = Utils::parse_client_max_body_size(client_max_body_size);

	if (size > 10000000)
	{
		Log::log("error: " + client_max_body_size + "client max body size too high, capping to 10M", STD_ERR | ERROR_FILE);
		return 10000000;
	}
	else if (size == -1)
	{
		Log::log("error: client max body size '" + client_max_body_size + "' is not valid, falling back to default (1M)\n", STD_ERR | ERROR_FILE);
		return CLIENT_MAX_BODY_SIZE_DEFAULT;
	}
	else
	{
		return size;
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