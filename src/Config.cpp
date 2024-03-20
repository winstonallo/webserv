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

			handle_server_names(it->second, new_server);
			handle_port(it->second, new_server);
			handle_access_log(it->second, new_server);
			handle_host(it->second, new_server);
			handle_client_max_body_size(it->second, new_server);

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

void	Config::handle_host(server_map& server, ServerInfo* new_server)
{
	if (server.find("host") == server.end() or server["host"].empty() == true)
	{
		throw std::runtime_error("error: missing host in server '" + new_server->get_server_name()[0] + "', server will not be initialized\n");
	}

	std::string host = server["host"][0];

	struct in_addr	ip_address;

	if (host == "localhost")
	{
		host = "127.0.0.1";
	}
	if (std::find(_hosts.begin(), _hosts.end(), host) != _hosts.end())
	{
		throw std::runtime_error("error: '" + host + "': host address already taken, server will not be initialized\n");
	}
	else if (inet_pton(AF_INET, host.c_str(), &ip_address) != 1)
	{
		throw std::runtime_error("error: '" + host + "' is not a valid IPv4 address, server will not be initialized\n");
	}
	_hosts.push_back(host);

	new_server->set_host_address(ip_address);
}

void	Config::handle_server_names(server_map& server, ServerInfo* new_server)
{
	if (server.find("server_name") == server.end() or server["server_name"].empty() == true)
	{
		throw std::runtime_error("error: missing server_name, server will not be initialized\n");
	}

	std::vector <std::string> new_server_names = server["server_name"];

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

void	Config::handle_port(server_map& server, ServerInfo* new_server)
{
	if (server.find("port") == server.end() or server["port"].empty() == true)
	{
		throw std::runtime_error("error: missing port in server '" + new_server->get_server_name()[0] + "', server will not be initialized\n");
	}

	int port = std::atoi(server["port"][0].c_str());

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
void	Config::handle_access_log(server_map& server, ServerInfo* new_server)
{
	std::string access_log;

	if (server.find("access_log") != server.end() && server["access_log"].empty() == false)
	{
		access_log = server["access_log"][0];

		if (Utils::file_exists(access_log) == false or Utils::write_access(access_log) == true)
		{
			new_server->set_access_log(access_log);
			return ;
		}
	}
	Log::log("error: access log '" + access_log + "' could not be opened, falling back to 'access.log'\n", STD_ERR | ERROR_FILE);

	new_server->set_access_log(ACCESS_LOG_DEFAULT);
}

void	Config::handle_client_max_body_size(server_map& server, ServerInfo* new_server)
{
	if (server.find("client_max_body_size") == server.end() or server["client_max_body_size"].empty() == true)
	{
		Log::log("error: client max body size missing in server '" + new_server->get_server_name()[0] + "' is missing, falling back to default (1M)\n", STD_ERR | ERROR_FILE);
		new_server->set_client_max_body_size(CLIENT_MAX_BODY_SIZE_DEFAULT);
		return ;
	}

	std::string client_max_body_size = server["client_max_body_size"][0];
	int	size = Utils::parse_client_max_body_size(client_max_body_size);

	if (size > CLIENT_MAX_BODY_SIZE_MAX)
	{
		Log::log("error: " + client_max_body_size + "client max body size too high, capping to 10M\n", STD_ERR | ERROR_FILE);
		size = CLIENT_MAX_BODY_SIZE_MAX;
	}
	else if (size == -1)
	{
		Log::log("error: client max body size '" + client_max_body_size + "' is not valid, falling back to default (1M)\n", STD_ERR | ERROR_FILE);
		size = CLIENT_MAX_BODY_SIZE_DEFAULT;
	}
	new_server->set_client_max_body_size(size);
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