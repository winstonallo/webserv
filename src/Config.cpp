/* 
	config class implementation
	handles loading, parsing, and accessing config settings from a file
 */

#include "../inc/Config.hpp"
#include <sstream>
#include <string>
#include <variant>
#include <vector>

// default constructor: loads config from path - "default_config.conf" if no param
Config::Config(const std::string& path)
{
	load_config(path);
}

void	Config::load_config(const std::string& path)
{
	std::ifstream				config_file(path.c_str());
	std::stringstream			buffer;
	std::string					line;
	std::string					dont_ask_rn;
	std::vector <std::string> 	keys(4, "");

	buffer << config_file.rdbuf();
	std::vector <std::string> vec = Parser::split_keep_delimiters(buffer.str(), "{};");
	for (std::vector<std::string>::iterator it = vec.begin(); it != vec.end(); it++)
		std::cout << (*it) << std::endl;
	nesting_level.push(Parser::trim(Parser::trim_comment(line, "#"), " \t\n"));
	while (std::getline(config_file, line, '{'))
	{
		keys[nesting_level.size() - 1] = nesting_level.top();
		max_nesting_level = std::max(nesting_level.size(), max_nesting_level); // keep track of nesting level
	}
	config_file.close();
}

// returns value from the config map
//
// @param 	primary_key: primary key for required value (TOP_LEVEL for top level values)
// @param 	secondary_key: secondary key for required value (eg: 'listen'for the port)
// @return	value: requested value
// std::vector <std::string>	Config::get_value(const std::string& primary_key, const std::string& secondary_key)
// {
// 	return config[primary_key][secondary_key];
// }

// returns the config map
config_map	Config::get_config() const
{
	return config;
}

// output operator overload for debugging
std::ostream& operator<<(std::ostream& os, const Config& config) 
{
	config_map map = config.get_config();
	for (config_map::iterator it = map.begin(); it != map.end(); ++it)
	{
		std::cout << BOLD << UNDERLINE << it->first << RESET << "\n" << std::endl;
		for (std::map <std::string, std::map <std::string, std::map <std::string, std::vector <std::string> > > >::iterator yo = it->second.begin(); yo != it->second.end(); yo++)
		{
			std::cout << BOLD << UNDERLINE << yo->first << RESET << "\n" << std::endl;
			for (std::map <std::string, std::map <std::string, std::vector <std::string> > >::iterator i = yo->second.begin(); i != yo->second.end(); i++)
			{
				std::cout << BOLD << UNDERLINE << i->first << RESET << "\n" << std::endl;
				for (std::map<std::string, std::vector<std::string> >::iterator it2 = i->second.begin(); it2 != i->second.end(); ++it2)
				{
					std::cout << "\t" << BOLD << it2->first << RESET << std::endl;
					for (std::vector<std::string>::iterator it3 = it2->second.begin(); it3 != it2->second.end(); ++it3)
					{
						std::cout << "\t\t" << *it3 << std::endl;
					}
				}
			}
		}
	}
	return os;
}

Config::~Config() {}

Config::Config(const Config& rhs) {*this = rhs;}

Config&	Config::operator=(const Config& rhs)
{
	if (this != &rhs)
		config = rhs.config;
	return (*this);
}