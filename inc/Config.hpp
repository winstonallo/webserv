#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "LocationInfo.hpp"
#include "ServerInfo.hpp"
#include "Route.hpp"
#include "CGI.hpp"
#include <map>
#include <netinet/in.h>
#include <string>
#include <vector>

typedef std::map <std::string, std::vector <std::string> > _map;

class Config
{
	public:

		typedef void (Route::*route_setter)(const std::vector <std::string>&);
		typedef std::map <std::string, route_setter> route_setter_map;
		typedef void (CGI::*cgi_setter)(const std::vector <std::string>&);
		typedef std::map <std::string, cgi_setter> cgi_setter_map;
		typedef void (LocationInfo::*location_setter)(const std::vector <std::string>&);
		typedef std::map <std::string, location_setter> location_setter_map;

		std::vector <ServerInfo *>				get_servers() const;
		std::vector <Route *>					get_routes() const;
		std::vector <CGI *>						get_cgi() const;
		std::string								get_error_page(const int key);

		void									set_servers(std::map <int, _map>& raw_servers);
		void									set_error_pages(const std::map <int, std::string>& error_pages);
		void									set_routes(const std::map <std::string, _map>& raw_routes);

		void									initialize_standard_route_setters();
		void									initialize_cgi_setters();
		void									initialize_location_setters();

		void									configure_port(_map& server, ServerInfo* new_server, std::vector <std::string>& new_unique_values);
		void									configure_server_names(_map& server, ServerInfo* new_server, std::vector <std::string>& new_unique_values);
		void									configure_host(_map& server, ServerInfo* new_server, std::vector <std::string>& new_unique_values);
		void									configure_access_log(_map& server, ServerInfo* new_server);
		void									configure_client_max_body_size(_map& server, ServerInfo* new_server);
		void									configure_locations(const _map& server, ServerInfo* new_server);
		location_setter_map::iterator			initialize_location_iteration(const std::string& name, const std::string& key, LocationInfo*& new_location);
		std::string								extract_location_path(const std::string& current_map_key);
		void									configure_standard_route(const _map &route, const std::string& name);
		void									configure_cgi(const _map &route);
		cgi_setter_map::iterator				initialize_cgi_iteration(const std::string& current_map_key, CGI*& new_cgi);

		Config(const std::string& config_path="config_files/webserv.conf");
		~Config();
	
	private:

		std::vector <CGI *>						_cgi;
		std::vector <Route *>					_routes;
		std::vector <ServerInfo *>				_servers;
		std::vector <LocationInfo *>			_locations;
		std::map <int, std::string>				_error_pages;

		std::map <int, std::string>             _error_status_codes;

		std::vector <std::string>				_unique_values;

		route_setter_map						_standard_route_setters;
		cgi_setter_map							_cgi_route_setters;
		location_setter_map						_location_setters;

		Config(const Config& rhs);
		Config &operator=(const Config& rhs);
};

#define CLIENT_MAX_BODY_SIZE_DEFAULT 1000000
#define CLIENT_MAX_BODY_SIZE_MAX 10000000
#define ACCESS_LOG_DEFAULT "access.log"


#endif