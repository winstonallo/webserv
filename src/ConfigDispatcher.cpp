#include "ConfigDispatcher.hpp"
#include <cerrno>
#include <algorithm>
#include <cstdlib>
#include <cstddef>
#include <fstream>
#include <ostream>
#include <sstream>
#include <string>
#include <utility>
#include "Utils.hpp"
#include "ServerInfo.hpp"
#include <map>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <stack>
#include "Log.hpp"
#include "Utils.hpp"

// default constructor, checks if config map is empty, then dipatches it
//
// @param raw_config:	config map from the parser
ConfigDispatcher::ConfigDispatcher(const std::map <std::string, std::vector <std::string> >& raw_config) : _status_codes(Utils::get_error_status_codes())
{
    if (raw_config.empty() == true)
    {
        throw std::runtime_error("cannot instantiate ConfigDispatcher with empty config");
    }
    _raw_config = raw_config;

    dispatch_values();
	generate_default_error_page(408);
}

// wrapper function for the dispatching of the values
//
// loops through the config and assignes the values to their corresponding maps
void	ConfigDispatcher::dispatch_values() 
{
	for (std::map <std::string, std::vector <std::string> >::iterator it = _raw_config.begin(); it != _raw_config.end(); it++)
	{
		handle_server(it->first);
		handle_error_page(std::make_pair(it->first, it->second));
		handle_route(it->first);
	}
}

// handles route config values
//
// @param key:	current config key
//
// if routes prefix ("webserv:routes") in key:
//		1. removes the prefix
// 		2. separates the top key (ex: "/admin", "/cgi-bin") from the rest of the substring
//		3. gets the final value key (full route - top key)
//		4. stores corresponding _raw_config values into the _routes map
void 	ConfigDispatcher::handle_route(const std::string& key)
{
	size_t routes_prefix_size = std::string(ROUTES_PREFIX).size();

	if (key.substr(0, routes_prefix_size) == ROUTES_PREFIX)
	{
		std::string full_route = key.substr(routes_prefix_size + 1);
		std::string route_key = full_route.substr(0, full_route.find_first_of(":"));
		size_t value_start_pos = route_key.length() + 1;
		std::string route_value_key = full_route.substr(value_start_pos);
		
		_routes[route_key][route_value_key] = _raw_config[key];
	}
}

// handles server config values
//
// @param key:	current config key
//
// the server key syntax is: "server_[0-9]", (since there can be multiple
// in one config, they get a number assigned during parsing)
//
// if server prefix ("webserv:server") in key:
//		1. extract the server id to use as a key in _server map
//		2. store corresponding value into _server map
void	ConfigDispatcher::handle_server(const std::string& key)
{
	size_t server_prefix_size = std::string(SERVER_PREFIX).size();

	if (key.substr(0, server_prefix_size) == SERVER_PREFIX)
	{
		int	server_id = Utils::extract_numeric_value(key.substr(server_prefix_size));

		_servers[server_id][key.substr(server_prefix_size + 3)] = _raw_config[key];
	}
}

// handles error page config values
//
// @param key_value:	current map key_value pair
//
// if error page prefix ("webserv:error_pages") in key_value.first:
//		1. extract status code from the key
//		2. 	if corresponding html file exists:
//				store into _error_pages map using status code as a key
//				erase corresponding status code from default status codes
//			else:
//				log error and fall back to default value (fallback to be implemented)
void    ConfigDispatcher::handle_error_page(const std::pair <std::string, std::vector <std::string> >& key_value)
{
    if (key_value.first.substr(0, key_value.first.find_last_of(":")) == ERROR_PAGE_PREFIX)
    {
        int status_code = std::atoi(key_value.first.substr(key_value.first.size() - 3).c_str());
        if (Utils::file_exists(key_value.second[0]) == true)
        {
            _error_pages[status_code] = key_value.second[0];
			_status_codes.erase(status_code);
        }
        else
        {
            Log::log("error: " + key_value.second[0] + ": file not found, falling back to default\n", ERROR_FILE);
        }
    }
}

void	ConfigDispatcher::generate_default_error_page(const int status_code)
{
	std::string 			old_error_code = "400", old_error_message = "bad request";
	std::ifstream			default_html(DEFAULT_ERROR_PAGE);

	if (default_html.is_open() == false)
	{
		throw std::runtime_error("error: could not open " + std::string(DEFAULT_ERROR_PAGE) + ": " +  strerror(errno));
	}

    std::stringstream buffer;
	buffer << default_html.rdbuf();
	std::string html = buffer.str();

	size_t pos = html.find(old_error_code);
	while (pos != std::string::npos)
	{
		std::string new_error_code = Utils::itoa(status_code);
		html.replace(pos, old_error_code.size(), new_error_code);
		pos = html.find(old_error_code, pos + new_error_code.size());
	}
	pos = html.find(old_error_message);
	while (pos != std::string::npos)
	{
		std::string new_error_message = _status_codes[status_code];
		html.replace(pos, old_error_message.size(), new_error_message);
		pos = html.find(old_error_message, pos + new_error_message.size());
	}
	std::cout << html << std::endl;
}

std::map <int, std::map <std::string, std::vector <std::string> > >		ConfigDispatcher::get_servers()
{
	return _servers;
}

void	ConfigDispatcher::print_error_pages()
{
	for (std::map <int, std::string>::iterator it = _error_pages.begin(); it != _error_pages.end(); it++)
	{
		std::cout << it->first << ": " << std::endl << "\t" << it->second << std::endl;
	}
}

void	ConfigDispatcher::print_servers()
{
	for (std::map <int, std::map <std::string, std::vector <std::string> > >::iterator it = _servers.begin(); it != _servers.end(); it++)
	{
		std::cout << it->first << ": " << std::endl;
		for (std::map <std::string, std::vector <std::string> >::iterator itt = it->second.begin(); itt != it->second.end(); itt++)
		{
			std::cout << "\t" << itt->first << ":" << std::endl;
			for (size_t i = 0; i < itt->second.size(); i++)
			{
				std::cout << "\t\t" << itt->second[i] << std::endl;
			}
		}
	}
}

void	ConfigDispatcher::print_routes()
{
	for (std::map <std::string, std::map <std::string, std::vector <std::string> > >::iterator it = _routes.begin(); it != _routes.end(); it++)
	{
		std::cout << it->first << ": " << std::endl;
		for (std::map <std::string, std::vector <std::string> >::iterator itt = it->second.begin(); itt != it->second.end(); itt++)
		{
			std::cout << "\t" << itt->first << ":" << std::endl;
			for (size_t i = 0; i < itt->second.size(); i++)
			{
				std::cout << "\t\t" << itt->second[i] << std::endl;
			}
		}
	}
}

ConfigDispatcher::~ConfigDispatcher() {}

ConfigDispatcher::ConfigDispatcher(const ConfigDispatcher& rhs) {*this = rhs;}

ConfigDispatcher    &ConfigDispatcher::operator=(const ConfigDispatcher& rhs)
{
    if (this != &rhs)
    {
        _raw_config = rhs._raw_config;
        _servers = rhs._servers;
        _error_pages = rhs._error_pages;
    }
    return *this;
}
