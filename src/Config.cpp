#include "Config.hpp"
#include "ConfigParser.hpp"
#include "LocationInfo.hpp"
#include "Server.hpp"
#include "Utils.hpp"
#include "ConfigSetters.hpp"
#include <algorithm>
#include <cstddef>
#include <exception>
#include <netinet/in.h>
#include <string>
#include <cstdlib>
#include <sys/socket.h>
#include <vector>
#include <arpa/inet.h>
#include "ConfigSetters.hpp"

// full initialization of the config in one constructor
// 
// HOW TO INITIALIZE:
//		Config config(optional: std::string config_path);
//		
//		@param path:	path to .conf file - if no path is specified, will use: "config_files/webserv.conf"
//
// ConfigParser:
//		1. 	reads the whole config
//		2. 	performs error handling on file structure
//		3. 	stores config in a one level map with the config path as key
//
// Config:
// 		1. 	gets error pages (already fully parsed in ConfigDispatcher 
//			since they are simple top level key-value pairs)
//		2.	performs final parsing and validation on routes & servers
//		3.	loads into Server, LocationInfo & Route
//			objects for server initialization
Config::Config(const std::string& config_path)
{
	ConfigParser 		parser(config_path);

	std::map <int, _map> servers = parser.get_servers();

	initialize_location_setters();
	initialize_server_setters();
	set_servers(servers);
}

void	Config::set_servers(std::map <int, _map>& raw_servers)
{
	Server* new_server;

	for (std::map <int, _map>::iterator it = raw_servers.begin(); it != raw_servers.end(); it++)
	{
		try
		{	
			new_server = new Server;

			configure_server_names(it->second, new_server);
			configure_port(it->second, new_server);
			configure_host(it->second, new_server);

			Utils::validate_required_server_values(new_server);

			configure_locations(it->second, new_server);

			for (_map::iterator map_it = it->second.begin(); map_it != it->second.end(); map_it++)
			{
				if (_server_setters.find(map_it->first) == _server_setters.end())
				{
					continue ;
				}
				(_server_setters[map_it->first])(map_it->second.first, new_server);
			}

			_servers.push_back(new_server);
		}
		catch (const std::exception& e)
		{
			delete new_server;

			for (std::vector <Server *>::iterator server = _servers.begin(); server != _servers.end(); server++)
			{
				delete *server;
			}
			for (std::vector <LocationInfo *>::iterator location = _locations.begin(); location != _locations.end(); location++)
			{
				delete *location;
			}
			throw std::runtime_error("Aborting initialization, please double-check your server configuration.\n");
		}
	}
}

void	Config::add_location(LocationInfo*& new_location, Server* new_server, bool add_to_server)
{
	if (new_location != NULL)
	{
		if (new_location->get_cgi() == true)
		{
			new_location->set_path("/cgi-bin");
		}
		_locations.push_back(new_location);
		new_location = NULL;
	}
	if (add_to_server == true)
	{
		new_server->set_locations(_locations);
		_locations.clear();
	}
}

Config::location_setter_map::iterator	Config::initialize_location(int line_count, const std::string& key, const std::string& name, LocationInfo*& new_location)
{
	if (new_location == NULL || name != new_location->get_path())
	{
		if (name != "/cgi-bin")
		{
			if (new_location != NULL)
			{
				_locations.push_back(new_location);
			}
			new_location = new LocationInfo;
			new_location->set_path(name);
		}
		else 
		{
			return configure_cgi(line_count, key, new_location);
		}
	}
	std::string current_key = key.substr(key.find_first_of(":") + 1);
	location_setter_map::iterator setter = _location_setters.find(current_key);

	return setter;
}

void	Config::configure_locations(const _map& server, Server*& new_server)
{
	LocationInfo*					new_location = NULL;

	for (_map::const_iterator it = server.begin(); it != server.end(); it++)
	{

		std::string name = Utils::extract_location_name(it->first);

		if (name.empty() == true)
		{
			continue ;
		}

		location_setter_map::iterator setter = initialize_location(it->second.second, it->first, name, new_location);
		
		if (setter != _location_setters.end())
		{
			try 
			{
				(setter->second)(it->second.first, new_location);
			}
			catch (const std::exception& e)
			{
				delete new_location;
				Utils::config_error_on_line(it->second.second, std::string(e.what()) + "Invalid configuration.");
			}
		}
		else
		{
			Utils::config_error_on_line(it->second.second, "'" + it->first + "' is not a valid location setting");
		}
	}
	add_location(new_location, new_server, true);
}

void	Config::initialize_cgi(LocationInfo*& new_location, const std::string& identifier)
{
	if (new_location != NULL)
	{
		if (identifier != new_location->get_path())
		{
			add_location(new_location);
			new_location = new LocationInfo;
		}
	}
	else
	{
		new_location = new LocationInfo;
	}
}

Config::location_setter_map::iterator	Config::configure_cgi(int line_count, const std::string key, LocationInfo*& new_location)
{
	std::string cgi_identifier = Utils::extract_cgi_identifier(key);
	if (cgi_identifier.empty() == true)
	{
		Utils::config_error_on_line(line_count, "missing CGI identifier in location '" + key + "', location will not be initialized");
		return _location_setters.end();
	}

	initialize_cgi(new_location, cgi_identifier);
	new_location->set_path(cgi_identifier);
	new_location->set_cgi(true);

	std::string current_key = key.substr(key.find_last_of(":") + 1);
	
	return _location_setters.find(current_key);
}

void	Config::configure_host(_map& server, Server*& new_server)
{
	if (server.find("host") == server.end() or server["host"].first.empty() == true)
	{
		Utils::config_error_on_line(-1, "missing host in server '" + new_server->get_server_name()[0] + "'.");
	}

	std::string host = server["host"].first[0];

	struct in_addr	ip_address;

	if (host == "localhost")
	{
		host = "127.0.0.1";
	}
	
	if (inet_pton(AF_INET, host.c_str(), &ip_address) != 1)
	{
		Utils::config_error_on_line(server["host"].second, "'" + host + "' is not a valid IPv4 address.");
	}

	new_server->set_host_address(ip_address);
}

void	Config::configure_server_names(_map& server, Server*& new_server)
{
	if (server.find("server_name") == server.end() or server["server_name"].first.empty() == true)
	{
		server["server_name"].first.push_back("default.com");
	}

	std::vector <std::string> new_server_names = server["server_name"].first;

	for (std::vector <std::string>::const_iterator it = new_server_names.begin(); it != new_server_names.end(); it++)
	{
		if (std::find(_server_names.begin(), _server_names.end(), *it) != _server_names.end())
		{
			Utils::config_error_on_line(server["server_name"].second, "on server: '" + *it + "': name already taken.");
		}
	}

	_server_names.insert(_server_names.begin(), new_server_names.begin(), new_server_names.end());

	new_server->set_server_name(new_server_names);

}

void	Config::configure_port(_map& server, Server*& new_server)
{
	if (server.find("port") == server.end() or server["port"].first.empty() == true)
	{
		Utils::config_error_on_line(-1, "missing port in server '" + new_server->get_server_name()[0] + "'.");	
	}

	std::string port = server["port"].first[0];

	new_server->set_port(std::atoi(port.c_str()));
}

void	Config::initialize_server_setters()
{
	_server_setters["access_log"] = &Setters::configure_access_log;
	_server_setters["client_max_body_size"] = &Setters::configure_client_max_body_size;
	_server_setters["autoindex"] = &Setters::configure_autoindex;
	_server_setters["root"] = &Setters::configure_root;
	_server_setters["index"] = &Setters::configure_index;
	_server_setters["error_pages"] = &Setters::add_error_page;
}

void	Config::initialize_location_setters()
{
	_location_setters["root"] = &Setters::set_root;
	_location_setters["allowed_methods"] = &Setters::set_allowed_methods;
	_location_setters["return"] = &Setters::set_return;
	_location_setters["alias"] = &Setters::set_alias;
	_location_setters["handler"] = &Setters::set_cgi_handler;
	_location_setters["extension"] = &Setters::set_cgi_extension;
	_location_setters["autoindex"] = &Setters::set_autoindex;
	_location_setters["index"] = &Setters::set_index;
}

Config::~Config() 
{
	for (std::vector <Server *>::iterator it = _servers.begin(); it != _servers.end(); it++)
	{
		delete *it;
	}
}
