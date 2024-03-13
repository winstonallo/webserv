#include "ConfigDispatcher.hpp"

ConfigDispatcher::ConfigDispatcher(const std::map <std::string, std::vector <std::string> >& raw_config)
{
    if (_raw_config.empty() == true)
    {
        throw std::runtime_error("cannot instantiate ConfigDispatcher with empty config");
    }
    _raw_config = raw_config;
    dispatch_values();
}

void	ConfigDispatcher::dispatch_values() 
{
	for (std::map <std::string, std::vector <std::string> >::iterator it = _raw_config.begin(); it != _raw_config.end(); it++)
	{
		if (it->first.substr(0, 14) == "webserv:server")
		{
			_servers[Utils::extract_numeric_value(it->first)][it->first.substr(it->first.find_last_of("0123456789") + 2)] = _raw_config[it->first];
		}
	}
}

void    ConfigDispatcher::handle_error_page(const std::pair <std::string, std::vector <std::string> >& key_value)
{
    if (key_value.first.substr(0, key_value.first.find_last_of(":")) == "webserv:error_pages")
    {
        int status_code = std::atoi(key_value.first.substr(key_value.first.size() - 3).c_str());
        if (Utils::file_exists(key_value.second[0]) == true)
        {
            _error_pages[status_code] = key_value.second[0];
        }
        else
        {
            // TODO: insert fallback logic here
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
