#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "LocationInfo.hpp"
#include "Server.hpp"
#include <map>
#include <netinet/in.h>
#include <ostream>
#include <string>
#include <vector>

class Config
{
	public:

		typedef std::map <std::string, std::pair <std::vector <std::string>, int> > _map;
		typedef void (*location_setter)(const std::vector <std::string>&, LocationInfo*&);
		typedef std::map <std::string, location_setter> location_setter_map;
		typedef void (*server_setter)(const std::vector <std::string>&, Server*&);
		typedef std::map <std::string, server_setter> server_setter_map;

		std::string								get_error_page(const int key);
		std::vector <Server *>					get_servers() const { return _servers; }

		Config(const std::string& config_path="assets/config/webserv.conf");
		~Config();
	
	private:

		void									set_servers(std::map <int, _map>& raw_servers);

		void									configure_port(_map& server, Server*& new_server);
		void									configure_server_names(_map& server, Server*& new_server);
		void									configure_host(_map& server, Server*& new_server);

		void									initialize_cgi(LocationInfo*& new_location, const std::string& identifier);
		location_setter_map::iterator			configure_cgi(int line_count, const std::string key, LocationInfo*& new_location);

		location_setter_map::iterator			initialize_location(int line_count, const std::string& key, const std::string& name, LocationInfo*& new_location);
		void									configure_locations(const _map& server, Server*& new_server);
		void									add_location(LocationInfo*& new_location, Server* new_server=NULL, bool add_to_server=false);

		void									initialize_location_setters();
		void									initialize_server_setters();

		std::vector <Server *>					_servers;
		std::map <int, std::string>				_error_pages;

		std::vector <LocationInfo *>			_locations;

		std::map <int, std::string>             _error_status_codes;

		location_setter_map						_location_setters;
		server_setter_map						_server_setters;

		std::vector <std::string>				_server_names;

		Config(const Config&) {}
		Config &operator=(const Config&) { return *this; };
};

std::ostream& operator<<(std::ostream& out, const Config& config);

#define MEGABYTE 1048576
#define CLIENT_MAX_BODY_SIZE_DEFAULT MEGABYTE
#define CLIENT_MAX_BODY_SIZE_MAX (16 * MEGABYTE)
#define MAX_URL_LENGTH 1024
#define MAX_HEADER_LENGTH 4096
#define ACCESS_LOG_DEFAULT "access.log"

#endif
