#include "ConfigDispatcher.hpp"
#include <cstddef>
#include <ostream>
#include <utility>
#include "Log.hpp"

ConfigDispatcher::ConfigDispatcher(const std::map <std::string, std::vector <std::string> >& raw_config)
{
    if (raw_config.empty() == true)
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
		handle_error_page(std::make_pair(it->first, it->second));
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
            Log::log("error: " + key_value.second[0] + ": file not found, falling back to default", ERROR_FILE);
        }
    }
}

void	ConfigDispatcher::print_error_pages()
{
	for (std::map <int, std::string>::iterator it = _error_pages.begin(); it != _error_pages.end(); it++)
	{
		std::cout << it->first << ": " << std::endl << "\t" << it->second << std::endl;
	}
}

void	ConfigDispatcher::print_servers()
{
	for (std::map <int, std::map <std::string, std::vector <std::string> > >::iterator it = _servers.begin(); it != _servers.end(); it++)
	{
		std::cout << it->first << ": " << std::endl;
		for (std::map <std::string, std::vector <std::string> >::iterator itt = it->second.begin(); itt != it->second.end(); itt++)
		{
			std::cout << "\t" << itt->first << ":" << std::endl;
			for (size_t i = 0; i < itt->second.size(); i++)
			{
				std::cout << "\t\t" << itt->second[i] << std::endl;
			}
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
