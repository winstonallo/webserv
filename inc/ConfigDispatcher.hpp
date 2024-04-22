
#ifndef CONFIGDISPATCHER_HPP
#define CONFIGDISPATCHER_HPP

#include <map>
#include <string>
#include <vector>

class ConfigDispatcher
{
    public:
        typedef std::map <std::string, std::pair <std::vector <std::string>, int> > map_with_line;
        void                                                                	dispatch_values();

        void                                                                	handle_error_page(const std::pair<std::string, std::pair <std::vector<std::string>, int> >& key_value);
		void																	handle_server(const std::string& key);
		void																	handle_route(const std::string& key);

		std::map <int, std::string>                                      		get_error_pages() const { return _error_pages; }
		std::map <int, map_with_line >				                            get_servers() const { return _servers; }

        ConfigDispatcher(const map_with_line& raw_config);
        ~ConfigDispatcher() {}

    private:

        map_with_line                                                       	_raw_config;

        std::map <int, map_with_line >			                                _servers;
        std::map <int, std::string>                                         	_error_pages;

        ConfigDispatcher(const ConfigDispatcher&) {}
        ConfigDispatcher& operator=(const ConfigDispatcher&) { return *this; }
};

#define SERVER_PREFIX "webserv:server"
#define ERROR_PAGE_PREFIX "webserv:error_pages"
#define ROUTES_PREFIX "webserv:routes"

#endif