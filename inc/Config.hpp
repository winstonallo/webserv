#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "LocationInfo.hpp"
#include "ServerInfo.hpp"
#include <map>

class Config
{
	public:

		Config();
		~Config();
	
	private:

		std::vector <ServerInfo>				_servers;
		std::map <int, std::string>				_error_pages;

		Config(const Config& rhs);
		Config &operator=(const Config& rhs);
};

#endif