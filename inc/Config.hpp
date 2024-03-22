#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "ServerInfo.hpp"
#include "Route.hpp"
#include <map>
#include <netinet/in.h>
#include <string>

typedef std::map <std::string, std::vector <std::string> > _map;

class Config
{
	public:

		std::vector <ServerInfo *>				get_servers() const;
		std::vector <Route *>					get_routes() const;
		std::string								get_error_page(const int key);

		void									set_servers(std::map <int, std::map <std::string, std::vector <std::string> > >& raw_servers);
		void									set_error_pages(std::map <int, std::string>& error_pages);
		void									set_routes(std::map <std::string, _map>& raw_routes);

		void									handle_port(_map& server, ServerInfo* new_server, std::vector <std::string>& new_unique_values);
		void									handle_server_names(_map& server, ServerInfo* new_server, std::vector <std::string>& new_unique_values);
		void									handle_host(_map& server, ServerInfo* new_server, std::vector <std::string>& new_unique_values);
		void									handle_access_log(_map& server, ServerInfo* new_server);
		void									handle_client_max_body_size(_map& server, ServerInfo* new_server);
		void									handle_locations(_map& server, ServerInfo* new_server);

		std::string                             generate_default_error_page(const int status_code);

		Config(const std::string& config_path="config_files/webserv.conf");
		~Config();
	
	private:

		std::vector <ServerInfo *>				_servers;
		std::vector <Route *>					_routes;
		std::map <int, std::string>				_error_pages;

		std::map <int, std::string>             _error_status_codes;

		std::vector <std::string>				_unique_values;
		std::vector <int>						_ports;
		std::vector <std::string>				_server_names;
		std::vector <std::string>				_hosts;

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