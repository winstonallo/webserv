#include "Config.hpp"
#include "CGI.hpp"
#include "ConfigDispatcher.hpp"
#include "ConfigParser.hpp"
#include "LocationInfo.hpp"
#include "Route.hpp"
#include "ServerInfo.hpp"
#include "Utils.hpp"
#include <algorithm>
#include <cstddef>
#include <exception>
#include <netinet/in.h>
#include <stdexcept>
#include <string>
#include "Log.hpp"
#include <cstdlib>
#include <sys/socket.h>
#include <vector>
#include <arpa/inet.h>

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
// ConfigDispatcher:
//		1. 	splits the config into logical parts
//			a. servers
//			b. routes
//			c. error pages
//
// Config:
// 		1. 	gets error pages (already fully parsed in ConfigDispatcher 
//			since they are simple top level key-value pairs)
//		2.	performs final parsing and validation on routes & servers
//		3.	loads into ServerInfo, LocationInfo & Route
//			objects for server initialization
Config::Config(const std::string& config_path)
{
	ConfigParser 		parser(config_path);
	ConfigDispatcher 	dispatcher(parser.get_config());

	std::map <int, _map> servers = dispatcher.get_servers();
	std::map <std::string, _map> routes = dispatcher.get_routes();

	_error_pages = dispatcher.get_error_pages();
	_error_status_codes = Utils::get_error_status_codes();
	initialize_standard_route_setters();
	initialize_cgi_setters();
	initialize_location_setters();
	set_routes(routes);
	set_servers(servers);
}

// takes the parsed servers from the config dispatcher and initializes the servers one by one
//
// if:	any of the required fields are missing
//		->	log error & fall back to default values
//
// else if: a unique value is already taken (host, port, server name)
//		->	log error & skip the server
//
// else:
//		->	initialize the server
//		->	add the server to the list of servers
//		->	add the unique values to the list of unique values
void	Config::set_servers(std::map <int, std::map <std::string, std::vector <std::string> > >& raw_servers)
{
	ServerInfo* new_server;

	for (std::map <int, _map>::iterator it = raw_servers.begin(); it != raw_servers.end(); it++)
	{
		try
		{	
			new_server = new ServerInfo();
			std::vector <std::string>	new_unique_values;

			configure_server_names(it->second, new_server, new_unique_values);
			configure_port(it->second, new_server, new_unique_values);
			configure_access_log(it->second, new_server);
			configure_host(it->second, new_server, new_unique_values);
			configure_client_max_body_size(it->second, new_server);
			configure_locations(it->second, new_server);

			_servers.push_back(new_server);
			_unique_values.insert(_unique_values.end(), new_unique_values.begin(), new_unique_values.end());
		}
		catch (const std::exception& e)
		{
			delete new_server;

			Log::log(e.what(), STD_ERR | ERROR_FILE);

			continue ;
		}
	}
}

// initializes the locations for a server
//
// if:		the key is "location"
//			->	initialize a new location:
//
//		 if: 		the key is "root"
//					->	set the path for the new location
//		
//		 else if: the key is "directory_listing"
//					->	set the directory listing for the location
//		
//		 else if: the key is "allowed_methods"
//					->	set the allowed methods for the location
//		
//		 else:
//					->	log error & skip the value
void	Config::configure_locations(_map& server, ServerInfo* new_server)
{
	std::vector <LocationInfo*> locations;
	std::string					location_key_prefix = "location";
	LocationInfo*				new_location = NULL;

	for (_map::iterator it = server.begin(); it != server.end(); it++)
	{
		size_t found = it->first.find(location_key_prefix);

		if (found != std::string::npos)
		{
			size_t	colon = it->first.find(":", found);
			std::string	path = it->first.substr(found + location_key_prefix.size(), colon - (found + location_key_prefix.size()));
			
			if (new_location == NULL || new_location->get_name() != path)
			{
				if (new_location != NULL)
				{
					locations.push_back(new_location);
				}
				new_location = new LocationInfo;
				new_location->set_name(path);
			}

			std::string key = it->first.substr(it->first.find_first_of(":") + 1);
			location_setter_map::iterator setter = _location_setters.find(key);

			if (setter != _location_setters.end())
			{
				try 
				{
					(new_location->*(setter->second))(it->second);
				}
				catch (const std::exception& e)
				{
					Log::log(e.what(), STD_ERR | ERROR_FILE);
				}
			}
			else
			{
				Log::log("error: config value  at path " + it->first + "'" + it->second[0] + "' not recognized, will be ignored in server initialization\n");
			}
		}
	}
	if (new_location != NULL)
	{
		locations.push_back(new_location);
	}
	new_server->add_locations(locations);
}

// finds the host in the server map and performs some error handling on them 
// before storing them in the current ServerInfo
//
//	if:		no host in config
//			-> skip server in initialization
//
//	if:		host already given to another server
//			-> skip server in initialization
//
//	else:	
//			-> add host to vector of unique values
//			-> set host of current ServerInfo object
void	Config::configure_host(_map& server, ServerInfo* new_server, std::vector <std::string>& new_unique_values)
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
	if (std::find(_unique_values.begin(), _unique_values.end(), host) != _unique_values.end())
	{
		throw std::runtime_error("error: '" + host + "': host address already taken, server will not be initialized\n");
	}
	else if (inet_pton(AF_INET, host.c_str(), &ip_address) != 1)
	{
		throw std::runtime_error("error: '" + host + "' is not a valid IPv4 address, server will not be initialized\n");
	}
	new_unique_values.push_back(host);

	new_server->set_host_address(ip_address);
}

// finds the server_name(s) in the server map and performs some error handling on them 
// before storing them in the current ServerInfo
//
//	if:		no server name in config
//			-> skip server in initialization
//
//	if:		server name already given to another server
//			-> skip server in initialization
//
//	else:	
//			-> add server name(s) to vector of unique values
//			-> set server name(s) of current ServerInfo object
void	Config::configure_server_names(_map& server, ServerInfo* new_server, std::vector <std::string>& new_unique_values)
{
	if (server.find("server_name") == server.end() or server["server_name"].empty() == true)
	{
		throw std::runtime_error("error: missing server_name, server will not be initialized\n");
	}

	std::vector <std::string> new_server_names = server["server_name"];

	for (std::vector <std::string>::const_iterator it = new_server_names.begin(); it != new_server_names.end(); it++)
	{
		if (std::find(_unique_values.begin(), _unique_values.end(), *it) != _unique_values.end())
		{
			throw std::runtime_error("error: on server: '" + *it + "': name already taken, server will not be initialized\n");
		}
	}

	new_server->set_server_name(new_server_names);

	new_unique_values.insert(new_unique_values.end(), new_server_names.begin(), new_server_names.end());
}

// finds the port in the server map and performs some error handling on it before storing it in the current ServerInfo
//
//	if:		no port in config
//			-> skip server in initialization
//
//	if:		port already given to another server
//			-> skip server in initialization
//
//	else:	
//			-> add port to vector of unique values
//			-> set port of current ServerInfo object
void	Config::configure_port(_map& server, ServerInfo* new_server, std::vector <std::string>& new_unique_values)
{
	if (server.find("port") == server.end() or server["port"].empty() == true)
	{
		throw std::runtime_error("error: missing port in server '" + new_server->get_server_name()[0] + "', server will not be initialized\n");
	}

	std::string port = server["port"][0];

	if (std::find(_unique_values.begin(), _unique_values.end(), port) != _unique_values.end())
	{
		throw std::runtime_error("error: on server '" + new_server->get_server_name()[0] + "': port " + port + " already taken: '" + new_server->get_server_name()[0] + "' will not be initialized\n");
	}
	new_server->set_port(std::atoi(port.c_str()));

	new_unique_values.push_back(port);
}

// validates access log from config
//
// 	if:		the file does not exist (meaning we can just create it)
// 	or:		it exists and we have write access to it
//			->	config valid, return the filename
//
// 	else:
//			->	path invalid, log error & return default 
void	Config::configure_access_log(_map& server, ServerInfo* new_server)
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

// validates client max body size from config
//
// if:	the value is missing or empty
//		->	log error & fall back to default
//
// else if: the value is too high
//		->	log error & cap it to 10M
//
// else if: the value is invalid
//		->	log error & fall back to default
void	Config::configure_client_max_body_size(_map& server, ServerInfo* new_server)
{
	if (server.find("client_max_body_size") == server.end() or server["client_max_body_size"].empty() == true)
	{
		Log::log("no client max body size config in server '" + new_server->get_server_name()[0] + "', falling back to default (1M)\n", STD_ERR | ERROR_FILE);
		new_server->set_client_max_body_size(CLIENT_MAX_BODY_SIZE_DEFAULT);
		return ;
	}

	std::string client_max_body_size = server["client_max_body_size"][0];
	int	size = Utils::parse_client_max_body_size(client_max_body_size);

	if (size > CLIENT_MAX_BODY_SIZE_MAX)
	{
		Log::log("error: " + client_max_body_size + ": client max body size too high, capping to 10M\n", STD_ERR | ERROR_FILE);
		size = CLIENT_MAX_BODY_SIZE_MAX;
	}
	else if (size == -1)
	{
		Log::log("error: client max body size '" + client_max_body_size + "' is not valid, falling back to default (1M)\n", STD_ERR | ERROR_FILE);
		size = CLIENT_MAX_BODY_SIZE_DEFAULT;
	}
	new_server->set_client_max_body_size(size);
}

void	Config::configure_standard_route(_map& route, const std::string& name)
{
	Route* new_route = new Route;

	new_route->set_name(name);

	for (_map::iterator current_route = route.begin(); current_route != route.end(); current_route++)
	{
		if (_standard_route_setters.find(current_route->first) != _standard_route_setters.end())
		{
			try 
			{
				(new_route->*(_standard_route_setters[current_route->first]))(current_route->second);
			}
			catch (const std::exception& e)
			{
				Log::log(e.what(), STD_ERR | ERROR_FILE);
			}
		}
		else
		{
			Log::log("error: config value '" + current_route->first + "' not recognized, will be ignored in route initialization\n", STD_ERR | ERROR_FILE);
		}
	}
	_routes.push_back(new_route);
}

void Config::configure_cgi(_map& route) 
{
    CGI* new_cgi = NULL;

    for (_map::iterator it = route.begin(); it != route.end(); it++) 
	{
        std::string cgi_name = it->first.substr(0, it->first.find_first_of(":"));
        std::string key = it->first.substr(it->first.find_first_of(":") + 1);

        if (new_cgi == NULL || cgi_name != new_cgi->get_name()) 
		{
            if (new_cgi != NULL) 
			{
                _cgi.push_back(new_cgi);
            }
            new_cgi = new CGI;
            new_cgi->set_name(cgi_name);
        }

        cgi_setter_map::iterator setter = _cgi_route_setters.find(key);
		
		if (setter != _cgi_route_setters.end()) 
		{
            try 
			{
                (new_cgi->*(setter->second))(it->second);
            } 
			catch (const std::exception& e) 
			{
                Log::log(e.what(), STD_ERR | ERROR_FILE);
            }
        } 
		else 
		{
            Log::log("error: CGI config key '" + key + "' not recognized.\n", STD_ERR | ERROR_FILE);
        }
    }
    if (new_cgi != NULL) 
	{
        _cgi.push_back(new_cgi);
    }
}

// takes the parsed routes from the config dispatcher 
// and initializes the routes one by one
//
// TODO: configure_cgi()
// TODO: add extensive comments
void	Config::set_routes(std::map <std::string, _map>& raw_routes)
{
	for (std::map <std::string, _map>::iterator it = raw_routes.begin(); it != raw_routes.end(); it++)
	{
		std::string name = it->first;

		if (name == "/cgi-bin")
		{
			configure_cgi(it->second);
		}
		else 
		{
			configure_standard_route(it->second, name);
		}
	}
}

std::vector <ServerInfo *>	Config::get_servers() const
{
	return _servers;
}

std::vector <Route *>	Config::get_routes() const
{
	return _routes;
}

// returns the error page for a given status code
//
// if:	the status code is not found in the error pages from the config
//		->	generate a default error page
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

// generates a default error page for a given status code
//
// cat DEFAULT_ERROR_PAGE | sed 's/400/XXX/g' | sed 's/bad request/new message/g' > new_html_path
std::string	Config::generate_default_error_page(const int status_code)
{
	std::string default_error_code = "400", default_error_message = "bad request", default_html = DEFAULT_ERROR_PAGE;
	std::string	new_error_code = Utils::itoa(status_code), new_error_message = _error_status_codes[status_code];

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

	if (oss == false)
	{
		Log::log("error: could not create default error page, falling back to 400: bad_request\n", STD_ERR | ERROR_FILE);
		return DEFAULT_ERROR_PAGE;
	}

	oss << default_html;
	return new_html_path;
}

void	Config::initialize_cgi_setters()
{
	_cgi_route_setters["extension"] = &CGI::set_extension;
	_cgi_route_setters["handler"] = &CGI::set_handler;
	_cgi_route_setters["allowed_methods"] = &CGI::set_allowed_methods;
}

void	Config::initialize_standard_route_setters()
{
	_standard_route_setters["allowed_methods"] = &Route::set_allowed_methods;
	_standard_route_setters["default_file"] = &Route::set_default_file;
	_standard_route_setters["root"] = &Route::set_root;
	_standard_route_setters["upload_directory"] = &Route::set_upload_directory;
	_standard_route_setters["http_redirect"] = &Route::set_http_redirect;
	_standard_route_setters["accept_file_upload"] = &Route::set_accept_file_upload;
	_standard_route_setters["directory_listing"] = &Route::set_directory_listing;
}

void	Config::initialize_location_setters()
{
	_location_setters["root"] = &LocationInfo::set_root;
	_location_setters["directory_listing"] = &LocationInfo::set_directory_listing;
	_location_setters["allowed_methods"] = &LocationInfo::set_allowed_methods;
}

Config::~Config() 
{
	for (std::vector <ServerInfo *>::iterator it = _servers.begin(); it != _servers.end(); it++)
	{
		delete *it;
	}
	for (std::vector <Route *>::iterator it = _routes.begin(); it != _routes.end(); it++)
	{
		delete *it;
	}
	for (std::vector<CGI *>::iterator it = _cgi.begin(); it != _cgi.end(); it++)
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