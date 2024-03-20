#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "ServerInfo.hpp"
#include <map>
#include <netinet/in.h>
#include <string>

class Config
{
	public:

		std::vector <ServerInfo *>				get_servers();
		std::string								get_error_page(const int key);
	


		void									set_servers(std::map <int, std::map <std::string, std::vector <std::string> > >& raw_servers);
		void									set_error_pages(std::map <int, std::string>& error_pages);
		void									set_routes(std::map <std::string, std::map <std::string, std::vector <std::string> > >& raw_routes);


		void									handle_port(const int port, ServerInfo* new_server);
		void									handle_server_names(const std::vector <std::string>& server_names, ServerInfo* new_server);
		void									handle_host(std::string& hostname, ServerInfo* new_server);
		std::string								handle_access_log(std::map <std::string, std::vector <std::string> >& server);
		int										handle_client_max_body_size(const std::string& client_max_body_size);

		std::string                             generate_default_error_page(const int status_code);

		Config(const std::map <int, std::string> error_pages);
		~Config();
	
	private:

		std::vector <ServerInfo *>				_servers;
		std::map <int, std::string>				_error_pages;

		std::map <int, std::string>             _status_codes;

		std::vector <int>						_ports;
		std::vector <std::string>				_server_names;
		std::vector <std::string>				_hosts;

		Config(const Config& rhs);
		Config &operator=(const Config& rhs);
};

#define CLIENT_MAX_BODY_SIZE_DEFAULT 1000000
#define ACCESS_LOG_DEFAULT "access.log"
#define DEFAULT_ERROR_PAGE "files/error_pages/400.html"

#endif