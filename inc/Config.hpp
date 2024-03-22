#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "LocationInfo.hpp"
#include "ServerInfo.hpp"
#include "Route.hpp"
#include "CGI.hpp"
#include <map>
#include <netinet/in.h>
#include <string>

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
		std::string								get_error_page(const int key);

		void									set_servers(std::map <int, std::map <std::string, std::vector <std::string> > >& raw_servers);
		void									set_error_pages(std::map <int, std::string>& error_pages);
		void									set_routes(std::map <std::string, _map>& raw_routes);

		void									initialize_standard_route_setters();
		void									initialize_cgi_setters();
		void									initialize_location_setters();

		void									configure_port(_map& server, ServerInfo* new_server, std::vector <std::string>& new_unique_values);
		void									configure_server_names(_map& server, ServerInfo* new_server, std::vector <std::string>& new_unique_values);
		void									configure_host(_map& server, ServerInfo* new_server, std::vector <std::string>& new_unique_values);
		void									configure_access_log(_map& server, ServerInfo* new_server);
		void									configure_client_max_body_size(_map& server, ServerInfo* new_server);
		void									configure_locations(_map& server, ServerInfo* new_server);
		void									configure_standard_route(_map &route, const std::string& name);
		void									configure_cgi(_map &route);

		std::string                             generate_default_error_page(const int status_code);

		Config(const std::string& config_path="config_files/webserv.conf");
		~Config();
	
	private:

		std::vector <CGI *>						_cgi;
		std::vector <Route *>					_routes;
		std::vector <ServerInfo *>				_servers;
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
#define DEFAULT_ERROR_PAGE "<!DOCTYPE html>\n\
<html lang=\"en\">\n\
<head>\n\
<meta charset=\"UTF-8\">\n\
<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n\
<title>400</title>\n\
<link href=\"error.css\" rel=\"stylesheet\">\n\
</head>\n\
<body>\n\
<div class=\"container bsod\">\n\
  <div class=\"neg\">\n\
    <h1 class=\"bg title\">error 400</h1>\n\
    <p>bad request</p>\n\
  </div>\n\
  <div class=\"nav\">\n\
    <a href=\"/\" class=\"link\">go home</a>\n\
    <a href=\"/contact\" class=\"link\">contact us</a>\n\
  </div>\n\
</div>\n\
</body>\n\
</html>"

#endif