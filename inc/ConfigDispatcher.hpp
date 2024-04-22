// class responsible for the validation & loading of parsed values into their corresponding object
// eg: Server, LocationInfo, etc

#ifndef CONFIGDISPATCHER_HPP
#define CONFIGDISPATCHER_HPP

#include <map>
#include <string>
#include <vector>

class ConfigDispatcher
{
    public:

        void                                                                			dispatch_values();

        void                                                                			handle_error_page(const std::pair <std::string, std::vector <std::string> >& key_value);
		void																			handle_server(const std::string& key);
		void																			handle_route(const std::string& key);

		std::map <int, std::string>                                      				get_error_pages() const { return _error_pages; }
		std::map <int, std::map <std::string, std::vector <std::string> > >				get_servers() const { return _servers; }

        ConfigDispatcher(const std::map <std::string, std::vector <std::string> >& raw_config);
        ~ConfigDispatcher() {}

    private:

        std::map <std::string, std::vector <std::string> >                  			_raw_config;

        std::map <int, std::map <std::string, std::vector <std::string> > > 			_servers;
        std::map <int, std::string>                                         			_error_pages;
		std::map <std::string, std::map <std::string, std::vector <std::string> > > 	_routes;

        ConfigDispatcher(const ConfigDispatcher&) {}
        ConfigDispatcher& operator=(const ConfigDispatcher&) { return *this; }
};

#define SERVER_PREFIX "webserv:server"
#define ERROR_PAGE_PREFIX "webserv:error_pages"
#define ROUTES_PREFIX "webserv:routes"

#endif